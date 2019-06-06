#include "Block.hpp"

namespace UrSQL {

Block::Block() :
	Block(BlockType::free_type)
{
}

Block::Block(BlockType aType) :
	m_type(aType)
{
	memset(m_data, 0, defaultPayloadSize);
}

Block::Block(Storable& aStorable)
{
	aStorable.encode(*this);
}

Block::Block(const Block& rhs) :
	m_type(rhs.m_type)
{
	memcpy(m_data, rhs.m_data, defaultPayloadSize);
}

Block& Block::operator=(const Block& rhs) noexcept {
	m_type = rhs.m_type;
	memcpy(m_data, rhs.m_data, defaultPayloadSize);
	return *this;
}

} /* Block.hpp */