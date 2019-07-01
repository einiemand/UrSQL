#include "Row.hpp"
#include "BufferStream.hpp"

namespace UrSQL {

Row::Row(blocknum_t aBlocknum) :
	MonoStorable(aBlocknum),
	m_data()
{
}

void Row::serialize(BufferWriter& aWriter) const {
	aWriter << static_cast<size_type>(m_data.size());
	for (const auto& thePair : m_data) {
		aWriter << thePair.first << thePair.second;
	}
}

void Row::deserialize(BufferReader& aReader) {
	size_type theColumnSize;
	aReader >> theColumnSize;
	for (; theColumnSize > 0; --theColumnSize) {
		std::string theFieldName;
		aReader >> theFieldName;
		Value theValue;
		aReader >> theValue;
		addField(std::move(theFieldName), std::move(theValue));
	}
}

BlockType Row::expectedBlockType() const {
	return BlockType::row_type;
}

void Row::addField(std::string aFieldName, Value aValue) {
	if (m_data.count(aFieldName)) {
		throw std::runtime_error("Impossible: a row has fields of the same name");
	}
	m_data.insert({ std::move(aFieldName),std::move(aValue) });
}

void Row::updateField(std::string aFieldName, Value aValue) {
	if (!m_data.count(aFieldName)) {
		throw std::runtime_error("Impossible: trying to update a field that doesn't exist");
	}
}

void RowCollection::addRow(Row aRow) {
	m_rows.emplace_back(std::move(aRow));
}

} /* UrSQL */