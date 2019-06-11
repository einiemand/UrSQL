#include "TOC.hpp"
#include "BufferStream.hpp"

namespace UrSQL {

TOC::TOC() :
	MonoStorable(0)
{
}

BlockType TOC::expected_block_type() const {
	return BlockType::TOC_type;
}

void TOC::serialize(BufferWriter& aWriter) const {
	size_type theMapSize = m_map.size();
	aWriter << theMapSize;

	for (const auto& thePair : m_map) {
		const std::string& theTableName = thePair.first;
		blocknum_t theBlockNum = thePair.second;
		aWriter << theTableName << theBlockNum;
	}
	//theResult.set_error(Error::block_fullData, "Block is full, cannot hold more data from TOC");
}

void TOC::deserialize(BufferReader& aReader) {
	size_type theMapSize;
	aReader >> theMapSize;

	for (; theMapSize > 0; --theMapSize) {
		std::string theTableName;
		blocknum_t theBlockNum;
		aReader >> theTableName >> theBlockNum;

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

}