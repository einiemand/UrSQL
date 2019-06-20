#include "Attribute.hpp"

namespace UrSQL {

AttributeBuilder::AttributeBuilder() :
	m_name(),
	m_type(ValueType::null_type),
	m_defaultValue(),
	m_isNullable(true),
	m_isPrimary(false),
	m_isAutoIncr(false)
{
}

void Attribute::serialize(BufferWriter& aWriter) const {

}

void Attribute::deserialize(BufferReader& aReader) {

}

} /* UrSQL */