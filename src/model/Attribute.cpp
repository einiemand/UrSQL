#include "model/Attribute.hpp"

#include "parser/Parser.hpp"
#include "parser/TokenStream.hpp"
#include "persistence/BufferStream.hpp"

namespace ursql {

void Attribute::serialize(BufferWriter& aWriter) const {
    aWriter << name_ << valueType_ << defaultValue_ << isNullable_ << isPrimary_
            << isAutoInc_;
}

void Attribute::deserialize(BufferReader& aReader) {
    aReader >> name_ >> valueType_ >> defaultValue_ >> isNullable_ >>
      isPrimary_ >> isAutoInc_;
}

void Attribute::setName(std::string name) {
    name_ = std::move(name);
}

void Attribute::setValueType(ValueType valueType) {
    valueType_ = valueType;
}

void Attribute::setDefaultValue(Value defaultValue) {
    defaultValue_ = std::move(defaultValue);
}

void Attribute::setNotNullable() {
    isNullable_ = false;
}

void Attribute::setPrimary() {
    isPrimary_ = true;
    setNotNullable();
}

void Attribute::setAutoInc() {
    isAutoInc_ = true;
    setNotNullable();
}

const std::string& Attribute::getName() const {
    return name_;
}

ValueType Attribute::getType() const {
    return valueType_;
}

const Value& Attribute::getDefaultValue() const {
    return defaultValue_;
}

bool Attribute::isNullable() const {
    return isNullable_;
}

bool Attribute::isPrimary() const {
    return isPrimary_;
}

bool Attribute::isAutoInc() const {
    return isAutoInc_;
}

bool Attribute::mustBeSpecified() const {
    return !isNullable() && getDefaultValue().isNull();
}

namespace {

ValueType parseNextValueType(TokenStream& ts) {
    if (ts.hasNext()) {
        switch (ts.next().get<TokenType::keyword>()) {
        case Keyword::integer_kw:
            return ValueType::int_type;
        case Keyword::float_kw:
            return ValueType::float_type;
        case Keyword::boolean_kw:
            return ValueType::bool_type;
        case Keyword::varchar_kw:
            return ValueType::varchar_type;
        default:
            (void)0;
        }
    }
    URSQL_THROW_NORMAL(MissingInput, "attribute type");
}

}  // namespace

Attribute Attribute::parse(TokenStream& ts) {
    Attribute attribute;
    attribute.setName(parser::parseNextIdentifier(ts));
    attribute.setValueType(parseNextValueType(ts));
    while (ts.hasNext() && ts.peek().getType() == TokenType::keyword) {
        Keyword keyword = ts.next().get<TokenType::keyword>();
        switch (keyword) {
        case Keyword::default_kw:
            attribute.setDefaultValue(Value::parse(ts));
            break;
        case Keyword::not_kw:
            URSQL_EXPECT(ts.skipIf(Keyword::null_kw), UnexpectedInput,
                         "'null' missing after 'not'");
            attribute.setNotNullable();
            break;
        case Keyword::primary_kw:
            URSQL_EXPECT(ts.skipIf(Keyword::key_kw), UnexpectedInput,
                         "'key' missing after 'primary'");
            attribute.setPrimary();
            break;
        case Keyword::auto_increment_kw:
            attribute.setAutoInc();
            break;
        default:
            URSQL_THROW_NORMAL(
              UnexpectedInput,
              std::format("unsupported keyword {} for an attribute", keyword));
        }
    }
    return attribute;
}

}  // namespace ursql
