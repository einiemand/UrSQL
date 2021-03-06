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

const Value& Row::getField(const std::string& aFieldName) const {
    URSQL_TRUTH(fieldExists(aFieldName),
                '\'' + aFieldName + "' doesn't exist in a row!");
    return m_data.at(aFieldName);
}

void Row::addField(std::string aFieldName, Value aValue) {
    URSQL_TRUTH(!m_data.count(aFieldName), "A row has fields of the same name");
    m_data.insert({ std::move(aFieldName), std::move(aValue) });
}

void Row::updateField(std::string aFieldName, Value aValue) {
    URSQL_TRUTH(m_data.count(aFieldName),
                "Trying to update a field that doesn't exist");
    m_data[std::move(aFieldName)] = std::move(aValue);
}

void RowCollection::addRow(std::unique_ptr<Row>&& aRow) {
    m_rows.push_back(std::move(aRow));
}

void RowCollection::eachRow(RowVisitor aVisitor) {
    for (auto iter = m_rows.begin(); iter != m_rows.end(); ++iter) {
        aVisitor(*iter->get());
    }
}

void RowCollection::eachRow(CleanRowVisitor aVisitor) const {
    for (auto iter = m_rows.cbegin(); iter != m_rows.cend(); ++iter) {
        aVisitor(*iter->get());
    }
}

void RowCollection::reorder(const Order& anOrder) {
    const std::string& theFieldName = anOrder.getFieldName();
    bool isDesc = anOrder.isDesc();
    if (isDesc) {
        std::sort(m_rows.begin(), m_rows.end(),
                  [&theFieldName](const std::unique_ptr<Row>& lhs,
                                  const std::unique_ptr<Row>& rhs) -> bool {
                      return lhs->getField(theFieldName) >
                             rhs->getField(theFieldName);
                  });
    }
    else {
        std::sort(m_rows.begin(), m_rows.end(),
                  [&theFieldName](const std::unique_ptr<Row>& lhs,
                                  const std::unique_ptr<Row>& rhs) -> bool {
                      return lhs->getField(theFieldName) <
                             rhs->getField(theFieldName);
                  });
    }
}

}  // namespace UrSQL
