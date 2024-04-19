#pragma once

#include <functional>
#include <unordered_map>

#include "Value.hpp"

namespace ursql {

class Row : public MonoStorable {
public:
    using DataMap = std::unordered_map<std::string, Value>;

    explicit Row(blocknum_t aBlocknum);
    ~Row() override = default;

    URSQL_DISABLE_COPY(Row);
    URSQL_DEFAULT_MOVE(Row);

    void serialize(BufferWriter& aWriter) const override;
    void deserialize(BufferReader& aReader) override;

    BlockType expectedBlockType() const override;

    inline bool fieldExists(const std::string& aFieldName) const {
        return m_data.find(aFieldName) != std::end(m_data);
    }

    const Value& getField(const std::string& aFieldName) const;
    void addField(std::string aFieldName, Value aValue);
    void updateField(const std::string& aFieldName, Value aValue);

private:
    DataMap m_data;
};

class Order;

class RowCollection {
public:
    using RowVisitor = std::function<void(Row&)>;
    using CleanRowVisitor = std::function<void(const Row&)>;

    RowCollection() = default;
    ~RowCollection() = default;

    RowCollection(const RowCollection&) = delete;
    RowCollection& operator=(const RowCollection&) = delete;

    void addRow(Row&& aRow);

    inline size_type size() const {
        return m_rows.size();
    }

    inline bool empty() const {
        return m_rows.empty();
    }

    void eachRow(RowVisitor aVisitor);
    void eachRow(CleanRowVisitor aVisitor) const;

    void reorder(const Order& anOrder);

private:
    std::vector<Row> m_rows;
};

}  // namespace ursql
