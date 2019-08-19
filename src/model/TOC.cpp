#include "model/TOC.hpp"
#include "model/BufferStream.hpp"

namespace UrSQL {

TOC::TOC() :
	MonoStorable(0),
	m_dirty(false),
	m_entityMap()
{
}

BlockType TOC::expectedBlockType() const {
	return BlockType::TOC_type;
}

void TOC::serialize(BufferWriter& aWriter) const {
	size_type theMapSize = m_entityMap.size();
	aWriter << theMapSize;

	for (const auto& thePair : m_entityMap) {
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

		m_entityMap.insert({ theEntityName,theBlocknum });
	}
}

blocknum_t TOC::getEntityPosByName(const std::string& anEntityName) const {
	URSQL_TRUTH(entityExists(anEntityName), "Check if entity exists before getting its block number!");
	return m_entityMap.at(anEntityName);
}

void TOC::add(const std::string& anEntityName, blocknum_t aBlocknum) {
	URSQL_TRUTH(!entityExists(anEntityName), "Check if entity exists before adding it to TOC!");
	m_entityMap.insert({ anEntityName,aBlocknum });
	makeDirty(true);
}

void TOC::drop(const std::string& anEntityName) {
	URSQL_TRUTH(entityExists(anEntityName), "Check if entity exists before dropping it from TOC!");
	m_entityMap.erase(anEntityName);
	makeDirty(true);
}

StringList TOC::collectEntityNames() const {
	StringList theEntityNames;
	for (const auto& thePair : m_entityMap) {
		theEntityNames.push_back(thePair.first);
	}
	return theEntityNames;
}

}