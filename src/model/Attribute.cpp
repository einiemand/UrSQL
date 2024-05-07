#include "model/Attribute.hpp"

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

void Attribute::setNullable(bool isNullable) {
    isNullable_ = isNullable;
}

void Attribute::setPrimary() {
    isPrimary_ = true;
}

void Attribute::setAutoInc() {
    isAutoInc_ = true;
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

}  // namespace ursql
