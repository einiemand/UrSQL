#include "model/Attribute.hpp"

#include "model/BufferStream.hpp"

namespace ursql {

Attribute::Attribute()
    : m_name(),
      m_type(ValueType::null_type),
      m_defaultValue(),
      m_isNullable(true),
      m_isPrimary(false),
      m_isAutoIncr(false) {}

void Attribute::serialize(BufferWriter& aWriter) const {
    aWriter << m_name << m_type << m_defaultValue << m_isNullable << m_isPrimary
            << m_isAutoIncr;
}

void Attribute::deserialize(BufferReader& aReader) {
    aReader >> m_name >> m_type >> m_defaultValue >> m_isNullable >>
      m_isPrimary >> m_isAutoIncr;
}

}  // namespace ursql
