#include "Storable.hpp"
#include "BufferStream.hpp"

namespace UrSQL {

MonoStorable::MonoStorable(blocknum_t aBlockNum) :
	Storable(),
	m_blocknum(aBlockNum)
{
}

void MonoStorable::encode(Block& aBlock) const {
	aBlock.set_type(expected_block_type());
	BufferWriter theWriter(aBlock.get_data(), defaultBlockSize);
	serialize(theWriter);
}

void MonoStorable::decode(const Block& aBlock, blocknum_t aBlockNum) {
	if (expected_block_type() != aBlock.get_type()) {
		throw std::runtime_error("Block type is NOT correct!");
	}
	set_blocknum(aBlockNum);
	BufferReader theReader(aBlock.get_data(), defaultBlockSize);
	deserialize(theReader);
}

}