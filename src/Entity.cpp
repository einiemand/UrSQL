#include "Entity.hpp"
#include "BufferStream.hpp"

namespace UrSQL {

BlockType Entity::expectedBlockType() const {
	return BlockType::entity_type;
}

void Entity::serialize(BufferWriter& aWriter) const {
	aWriter << static_cast<size_type>(m_attributes.size());
	for (const Attribute& theAttr : m_attributes) {
		aWriter << theAttr;
	}
}

void Entity::deserialize(BufferReader& aReader) {
	size_type theAttrCount;
	aReader >> theAttrCount;
	for (; theAttrCount > 0; --theAttrCount) {
		Attribute theAttr;
		aReader >> theAttr;
		m_attributes.emplace_back(std::move(theAttr));
	}
}

}