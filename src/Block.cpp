#include "Block.hpp"
#include "Storable.hpp"
#include "BufferStream.hpp"

namespace UrSQL {

Block::Block(BlockType aType) :
	m_type(aType),
	m_data("")
{
}

Block::Block(const MonoStorable& aMonoStorable) :
	m_type(BlockType::free_type),
	m_data("")
{
	aMonoStorable.encode(*this);
}

Block::Block(const Block& rhs) noexcept :
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