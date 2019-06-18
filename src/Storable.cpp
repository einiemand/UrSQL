#include "Storable.hpp"
#include "BufferStream.hpp"

namespace UrSQL {

MonoStorable::MonoStorable(blocknum_t aBlocknum) :
	Storable(),
	m_blocknum(aBlocknum)
{
}

void MonoStorable::encode(Block& aBlock) const {
	aBlock.setType(expectedBlockType());
	BufferWriter theWriter(aBlock.getData(), defaultBlockSize);
	serialize(theWriter);
}

void MonoStorable::decode(const Block& aBlock, blocknum_t aBlocknum) {
	if (expectedBlockType() != aBlock.getType()) {
		throw std::runtime_error("Block type is NOT correct!");
	}
	setBlocknum(aBlocknum);
	BufferReader theReader(aBlock.getData(), defaultBlockSize);
	deserialize(theReader);
}

}