#include "Entity.hpp"

namespace UrSQL {




BlockType Entity::expected_block_type() const {
	return BlockType::entity_type;
}

void Entity::serialize(BufferWriter& aWriter) const {

}

void Entity::deserialize(BufferReader& aReader) {

}

}