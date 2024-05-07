#include "model/Value.hpp"

#include <format>

#include "exception/InternalError.hpp"
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
        URSQL_UNREACHABLE(std::format("unknown value type: {}",
                                      static_cast<std::size_t>(type)));
    }
}

bool Value::keywordIsValueType(Keyword aKeyword) {
    switch (aKeyword) {
    case Keyword::integer_kw:
    case Keyword::float_kw:
    case Keyword::boolean_kw:
    case Keyword::varchar_kw:
        return true;
    default:
        return false;
    }
}

ValueType Value::keyword2ValueType(Keyword keyword) {
    switch (keyword) {
    case Keyword::integer_kw:
        return ValueType::int_type;
    case Keyword::float_kw:
        return ValueType::float_type;
    case Keyword::boolean_kw:
        return ValueType::bool_type;
    case Keyword::varchar_kw:
        return ValueType::varchar_type;
    default:
        URSQL_UNREACHABLE(
          std::format("unknown keyword: {}",
                      static_cast<std::underlying_type_t<Keyword>>(keyword)));
    }
}

}  // namespace ursql
