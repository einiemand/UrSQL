#pragma once
#ifndef ROW_HPP
#define ROW_HPP
#include "Value.hpp"
#include <unordered_map>
#include <functional>

namespace UrSQL {

class Row : public MonoStorable {
public:
	using DataMap = std::unordered_map<std::string, Value>;

	explicit Row(blocknum_t aBlocknum);
	~Row() override = default;

	URSQL_DEFAULT_MOVE_CTOR(Row);
	URSQL_DISABLE_COPY_ASSIGN(Row);

	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;

	BlockType expectedBlockType() const override;

	inline bool fieldExists(const std::string& aFieldName) const {
		return m_data.count(aFieldName) == 1;
	}
	const Value& getField(const std::string& aFieldName) const;
	void addField(std::string aFieldName, Value aValue);
	void updateField(std::string aFieldName, Value aValue);
private:
	DataMap m_data;
};

class Order;

class RowCollection {
public:
	using RowList = std::vector<std::unique_ptr<Row>>;
	using RowVisitor = std::function<void(Row&)>;
	using CleanRowVisitor = std::function<void(const Row&)>;

	RowCollection() = default;
	~RowCollection() = default;

	RowCollection(const RowCollection&) = delete;
	RowCollection& operator=(const RowCollection&) = delete;

	void addRow(std::unique_ptr<Row>&& aRow);

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
	RowList m_rows;
};

} /* UrSQL */

#endif /* Row.hpp */