#include "Block.hpp"
#include "BufferStream.hpp"

namespace UrSQL {

Block::Block(BlockType aType) :
	m_type(aType),
	m_data("")
{
}

Block::Block(const Storable& aStorable) :
	m_type(BlockType::free_type),
	m_data("")
{
	aStorable.encode(*this);
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

TOC::TOC() :
	Storable(0)
{
}

void TOC::encode(Block& aBlock) const {
	aBlock.set_type(BlockType::TOC_type);
	BufferWriter theWriter(aBlock.get_data(), defaultPayloadSize);

	size_type theMapSize = m_map.size();
	theWriter << theMapSize;

	for (const auto& thePair : m_map) {
		const std::string& theTableName = thePair.first;
		blocknum_t theBlockNum = thePair.second;
		theWriter << theTableName << theBlockNum;
	}
	//theResult.set_error(Error::block_fullData, "Block is full, cannot hold more data from TOC");
}

void TOC::decode(const Block& aBlock, blocknum_t aBlockNum) {
	if (aBlock.get_type() != BlockType::TOC_type) {
		throw std::runtime_error("Block type is not TOC!");
	}
	set_blocknum(aBlockNum);

	BufferReader theReader(aBlock.get_data(), defaultPayloadSize);
	size_type theMapSize;
	theReader >> theMapSize;

	for (; theMapSize > 0; --theMapSize) {
		std::string theTableName;
		blocknum_t theBlockNum;
		theReader >> theTableName >> theBlockNum;

		m_map.insert({ theTableName,theBlockNum });
	}
}

blocknum_t TOC::get_blocknum_by_name(const std::string& aTableName) const {
	if (table_exists(aTableName)) {
		return m_map.at(aTableName);
	}
	throw std::runtime_error("Check if table exists before getting its block number!");
}

void TOC::add(const std::string& aTableName, blocknum_t aBlockNum) {
	if (table_exists(aTableName)) {
		throw std::runtime_error("Check if table exists before adding it to TOC!");
	}
	m_map.insert({ aTableName,aBlockNum });
}

void TOC::drop(const std::string& aTableName) {
	if (!table_exists(aTableName)) {
		throw std::runtime_error("Check if table exists before dropping it from TOC!");
	}
	m_map.erase(aTableName);
}

} /* Block.hpp */