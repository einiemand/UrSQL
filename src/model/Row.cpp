#include "model/Row.hpp"

#include <algorithm>

#include "model/BufferStream.hpp"
#include "model/Order.hpp"

namespace UrSQL {

Row::Row(blocknum_t aBlocknum) : MonoStorable(aBlocknum), m_data() {}

void Row::serialize(BufferWriter& aWriter) const {
    aWriter << static_cast<size_type>(m_data.size());
    for (const auto& thePair : m_data) {
        aWriter << thePair.first << thePair.second;
    }
}

void Row::deserialize(BufferReader& aReader) {
    auto theColumnSize = aReader.read<size_type>();
    for (; theColumnSize > 0; --theColumnSize) {
        auto theFieldName = aReader.read<std::string>();
        auto theValue = aReader.read<Value>();
        addField(std::move(theFieldName), std::move(theValue));
    }
}

BlockType Row::expectedBlockType() const {
    return BlockType::row_type;
}

const Value& Row::getField(const std::string& aFieldName) const {
    URSQL_TRUTH(fieldExists(aFieldName),
                '\'' + aFieldName + "' doesn't exist in a row!");
    return m_data.at(aFieldName);
}

void Row::addField(std::string aFieldName, Value aValue) {
    URSQL_TRUTH(!m_data.count(aFieldName), "A row has fields of the same name");
    m_data.emplace(std::move(aFieldName), std::move(aValue));
}

void Row::updateField(const std::string& aFieldName, Value aValue) {
    auto it = m_data.find(aFieldName);
    URSQL_TRUTH(it != std::end(m_data),
                "Trying to update a field that doesn't exist");
    it->second = std::move(aValue);
}

void RowCollection::addRow(Row&& aRow) {
    m_rows.push_back(std::move(aRow));
}

void RowCollection::eachRow(RowVisitor aVisitor) {
    for (Row& theRow : m_rows) {
        aVisitor(theRow);
    }
}

void RowCollection::eachRow(CleanRowVisitor aVisitor) const {
    for (const Row& theRow : m_rows) {
        aVisitor(theRow);
    }
}

void RowCollection::reorder(const Order& anOrder) {
    const std::string& theFieldName = anOrder.getFieldName();
    bool isDesc = anOrder.isDesc();
    if (isDesc) {
        std::sort(m_rows.begin(), m_rows.end(),
                  [&theFieldName](const Row& lhs, const Row& rhs) -> bool {
                      return lhs.getField(theFieldName) >
                             rhs.getField(theFieldName);
                  });
    } else {
        std::sort(m_rows.begin(), m_rows.end(),
                  [&theFieldName](const Row& lhs, const Row& rhs) -> bool {
                      return lhs.getField(theFieldName) <
                             rhs.getField(theFieldName);
                  });
    }
}

}  // namespace UrSQL
