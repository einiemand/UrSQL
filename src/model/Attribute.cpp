#include "model/Attribute.hpp"

#include "model/BufferStream.hpp"

namespace UrSQL {

Attribute::Attribute()
    : m_name(),
      m_type(ValueType::null_type),
      m_defaultValue(),
      m_isNullable(true),
      m_isPrimary(false),
      m_isAutoIncr(false) {}

void Attribute::serialize(BufferWriter& aWriter) const {
    aWriter << m_name << static_cast<char>(m_type) << m_defaultValue
            << m_isNullable << m_isPrimary << m_isAutoIncr;
}

void Attribute::deserialize(BufferReader& aReader) {
    char typeChar;
    aReader >> m_name >> typeChar >> m_defaultValue >> m_isNullable >>
      m_isPrimary >> m_isAutoIncr;
    m_type = static_cast<ValueType>(typeChar);
}

}  // namespace UrSQL