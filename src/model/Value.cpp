#include "model/Value.hpp"

#include <format>

#include "common/Messaging.hpp"
#include "exception/InternalError.hpp"
#include "exception/UserError.hpp"
#include "parser/TokenStream.hpp"
#include "persistence/BufferStream.hpp"

namespace ursql {

namespace {

template<typename... T>
struct overloaded : T... {
    using T::operator()...;
};

template<typename... T>
overloaded(T...) -> overloaded<T...>;

}  // namespace

Value::Value(int_t intVal) noexcept(
  std::is_nothrow_constructible_v<
    Var, std::in_place_index_t<val_type_ordinal_v<ValueType::int_type>>, int_t>)
    : var_(std::in_place_index<val_type_ordinal_v<ValueType::int_type>>,
           intVal) {}

Value::Value(float_t floatVal) noexcept(
  std::is_nothrow_constructible_v<
    Var, std::in_place_index_t<val_type_ordinal_v<ValueType::float_type>>,
    float_t>)
    : var_(std::in_place_index<val_type_ordinal_v<ValueType::float_type>>,
           floatVal) {}

Value::Value(bool_t boolVal) noexcept(
  std::is_nothrow_constructible_v<
    Var, std::in_place_index_t<val_type_ordinal_v<ValueType::bool_type>>,
    bool_t>)
    : var_(std::in_place_index<val_type_ordinal_v<ValueType::bool_type>>,
           boolVal) {}

Value::Value(varchar_t varcharVal) noexcept(
  std::is_nothrow_constructible_v<
    Var, std::in_place_index_t<val_type_ordinal_v<ValueType::varchar_type>>,
    std::add_rvalue_reference_t<varchar_t>>)
    : var_(std::in_place_index<val_type_ordinal_v<ValueType::varchar_type>>,
           std::move(varcharVal)) {}

ValueType Value::getType() const {
    return static_cast<ValueType>(var_.index());
}

bool Value::isNull() const {
    return getType() == ValueType::null_type;
}

bool Value::castableTo(ValueType type) const {
    switch (getType()) {
    case ValueType::null_type:
        return true;
    case ValueType::int_type:
    case ValueType::float_type:
        return type == ValueType::int_type || type == ValueType::float_type;
    case ValueType::bool_type:
        return type == ValueType::bool_type;
    case ValueType::varchar_type:
        return type == ValueType::varchar_type;
    default:
        URSQL_UNREACHABLE(std::format("unknown cast type {}", type));
    }
}

Value Value::cast(ValueType type) const {
    return std::visit(
      overloaded{
        [](null_t) {
            return Value();
        },
        [type](int_t intVal) {
            switch (type) {
            case ValueType::int_type:
                return Value(intVal);
            case ValueType::float_type:
                return Value(static_cast<float_t>(intVal));
            default:
                URSQL_THROW_NORMAL(MisMatch, "column type and value type");
            }
        },
        [type](float_t floatVal) {
            switch (type) {
            case ValueType::int_type:
                return Value(static_cast<int_t>(floatVal));
            case ValueType::float_type:
                return Value(floatVal);
            default:
                URSQL_THROW_NORMAL(MisMatch, "column type and value type");
            }
        },
        [type](bool_t boolVal) {
            URSQL_EXPECT(type == ValueType::bool_type, MisMatch,
                         "column type and value type");
            return Value(boolVal);
        },
        [type](const varchar_t& varcharVal) {
            URSQL_EXPECT(type == ValueType::varchar_type, MisMatch,
                         "column type and value type");
            return Value(varcharVal);
        } },
      var_);
}

std::string Value::toString() const {
    return std::visit(overloaded{ [](auto&& val) {
                                     return std::to_string(val);
                                 },
                                  [](null_t) -> std::string {
                                      return "NULL";
                                  },
                                  [](bool_t boolVal) -> std::string {
                                      return boolVal ? "t" : "f";
                                  },
                                  [](const varchar_t& varcharVal) {
                                      return varcharVal;
                                  } },
                      var_);
}

std::size_t Value::displayWidth() const {
    return toString().length();
}

void Value::show(std::ostream& os) const {
    std::visit(overloaded{ [&](auto&& val) {
                              os << val;
                          },
                           [&](std::monostate) {
                               os << "NULL";
                           },
                           [&](bool_t boolVal) {
                               os << (boolVal ? 't' : 'f');
                           },
                           [&](const varchar_t&& varcharVal) {
                               os << varcharVal;
                           } },
               var_);
}

void Value::serialize(BufferWriter& writer) const {
    writer << getType();
    std::visit(overloaded{ [&](auto&& val) {
                              writer << val;
                          },
                           [](null_t) {} },
               var_);
}

void Value::deserialize(BufferReader& reader) {
    auto type = reader.read<ValueType>();
    switch (type) {
    case ValueType::null_type:
        var_.emplace<val_type_ordinal_v<ValueType::null_type>>();
        break;
    case ValueType::int_type:
        var_.emplace<val_type_ordinal_v<ValueType::int_type>>(
          reader.read<int_t>());
        break;
    case ValueType::float_type:
        var_.emplace<val_type_ordinal_v<ValueType::float_type>>(
          reader.read<float_t>());
        break;
    case ValueType::bool_type:
        var_.emplace<val_type_ordinal_v<ValueType::bool_type>>(
          reader.read<bool_t>());
        break;
    case ValueType::varchar_type:
        var_.emplace<val_type_ordinal_v<ValueType::varchar_type>>(
          reader.read<varchar_t>());
        break;
    default:
        URSQL_UNREACHABLE(std::format("unknown value type: {}", type));
    }
}

namespace {

Value parseKeywordValue(Keyword keyword) {
    switch (keyword) {
    case Keyword::null_kw:
        return Value();
    case Keyword::true_kw:
        return Value(true);
    case Keyword::false_kw:
        return Value(false);
    default:
        URSQL_THROW_NORMAL(
          UnexpectedInput,
          std::format("unsupported keyword {} used as Value", keyword));
    }
}

}  // namespace

Value Value::parse(TokenStream& ts) {
    auto& token = ts.next();
    switch (token.getType()) {
    case TokenType::keyword:
        return parseKeywordValue(token.get<TokenType::keyword>());
    case TokenType::number:
        return Value(token.get<TokenType::number>());
    case TokenType::text:
        return Value(token.get<TokenType::text>());
    default:
        URSQL_THROW_NORMAL(
          UnexpectedInput,
          std::format("unrecognized input as value {}", token.toString()));
    }
}

std::ostream& operator<<(std::ostream& os, const Value& val) {
    val.show(os);
    return os;
}

}  // namespace ursql
