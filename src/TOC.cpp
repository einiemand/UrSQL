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
		const std::string& theEntityName = thePair.first;
		blocknum_t theBlockNum = thePair.second;
		aWriter << theEntityName << theBlockNum;
	}
	//theResult.set_error(Error::block_fullData, "Block is full, cannot hold more data from TOC");
}

void TOC::deserialize(BufferReader& aReader) {
	size_type theMapSize;
	aReader >> theMapSize;

	for (; theMapSize > 0; --theMapSize) {
		std::string theEntityName;
		blocknum_t theBlockNum;
		aReader >> theEntityName >> theBlockNum;

		m_map.insert({ theEntityName,theBlockNum });
	}
}

blocknum_t TOC::get_blocknum_by_name(const std::string& anEntityName) const {
	if (entity_exists(anEntityName)) {
		return m_map.at(anEntityName);
	}
	throw std::runtime_error("Check if entity exists before getting its block number!");
}

void TOC::add(const std::string& anEntityName, blocknum_t aBlockNum) {
	if (entity_exists(anEntityName)) {
		throw std::runtime_error("Check if entity exists before adding it to TOC!");
	}
	m_map.insert({ anEntityName,aBlockNum });
}

void TOC::drop(const std::string& anEntityName) {
	if (!entity_exists(anEntityName)) {
		throw std::runtime_error("Check if entity exists before dropping it from TOC!");
	}
	m_map.erase(anEntityName);
}

}