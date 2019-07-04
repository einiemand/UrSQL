#pragma once
#ifndef ROW_HPP
#define ROW_HPP
#include <unordered_map>
#include "Value.hpp"

namespace UrSQL {

class Row : public MonoStorable {
public:
	using DataMap = std::unordered_map<std::string, Value>;

	Row(blocknum_t aBlocknum);
	~Row() override = default;

	Row(Row&&) = default;
	Row& operator=(const Row&) = delete;

	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;

	BlockType expectedBlockType() const override;

	void addField(std::string aFieldName, Value aValue);
	void updateField(std::string aFieldName, Value aValue);

	inline bool fieldExists(const std::string& aFieldName) const {
		return m_data.count(aFieldName) == 1;
	}
private:
	DataMap m_data;
};

class RowCollection {
public:
	using RowList = std::vector<Row>;

	RowCollection() = default;
	~RowCollection() = default;

	RowCollection(const RowCollection&) = delete;
	RowCollection& operator=(const RowCollection&) = delete;

	void addRow(Row aRow);

	inline size_type size() const {
		return m_rows.size();
	}
private:
	RowList m_rows;
};

} /* UrSQL */

#endif /* Row.hpp */