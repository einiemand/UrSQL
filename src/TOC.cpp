#include "TOC.hpp"
#include "BufferStream.hpp"

namespace UrSQL {

TOC::TOC() :
	MonoStorable(0)
{
}

BlockType TOC::expectedBlockType() const {
	return BlockType::TOC_type;
}

void TOC::serialize(BufferWriter& aWriter) const {
	size_type theMapSize = m_map.size();
	aWriter << theMapSize;

	for (const auto& thePair : m_map) {
		const std::string& theEntityName = thePair.first;
		blocknum_t theBlocknum = thePair.second;
		aWriter << theEntityName << theBlocknum;
	}
	//theResult.setError(Error::block_fullData, "Block is full, cannot hold more data from TOC");
}

void TOC::deserialize(BufferReader& aReader) {
	size_type theMapSize;
	aReader >> theMapSize;

	for (; theMapSize > 0; --theMapSize) {
		std::string theEntityName;
		blocknum_t theBlocknum;
		aReader >> theEntityName >> theBlocknum;

		m_map.insert({ theEntityName,theBlocknum });
	}
}

blocknum_t TOC::getBlocknumByName(const std::string& anEntityName) const {
	if (entityExists(anEntityName)) {
		return m_map.at(anEntityName);
	}
	throw std::runtime_error("Check if entity exists before getting its block number!");
}

void TOC::add(const std::string& anEntityName, blocknum_t aBlocknum) {
	if (entityExists(anEntityName)) {
		throw std::runtime_error("Check if entity exists before adding it to TOC!");
	}
	m_map.insert({ anEntityName,aBlocknum });
}

void TOC::drop(const std::string& anEntityName) {
	if (!entityExists(anEntityName)) {
		throw std::runtime_error("Check if entity exists before dropping it from TOC!");
	}
	m_map.erase(anEntityName);
}

}