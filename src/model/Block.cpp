#include "model/Block.hpp"
#include "model/Storable.hpp"
#include "model/BufferStream.hpp"

namespace UrSQL {

Block::Block(BlockType aType) :
	m_type(aType),
	m_Data("")
{
}

Block::Block(const MonoStorable& aMonoStorable) :
	m_type(BlockType::free_type),
	m_Data("")
{
	aMonoStorable.encode(*this);
}

Block::Block(const Block& rhs) noexcept :
	m_type(rhs.m_type)
{
	memcpy(m_Data, rhs.m_Data, defaultPayloadSize);
}

Block& Block::operator=(const Block& rhs) noexcept {
	m_type = rhs.m_type;
	memcpy(m_Data, rhs.m_Data, defaultPayloadSize);
	return *this;
}

} /* Block.hpp */