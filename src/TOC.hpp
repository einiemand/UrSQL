#pragma once
#ifndef TOC_HPP
#define TOC_HPP
#include "Storable.hpp"

namespace UrSQL {

class TOC : public MonoStorable {
public:
	using TableNumMap = std::unordered_map<std::string, blocknum_t>;

	TOC();
	~TOC() = default;

	BlockType expected_block_type() const override;
	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;

	inline bool table_exists(const std::string& aTableName) const {
		return m_map.count(aTableName) == 1;
	}

	blocknum_t get_blocknum_by_name(const std::string& aTableName) const;

	void add(const std::string& aTableName, blocknum_t aBlockNum);

	void drop(const std::string& aTableName);

private:
	TableNumMap m_map;
};

}

#endif