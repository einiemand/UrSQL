#pragma once
#ifndef TOC_HPP
#define TOC_HPP
#include "Storable.hpp"

namespace UrSQL {

class TOC : public MonoStorable {
public:
	using EntityNumMap = std::unordered_map<std::string, blocknum_t>;

	TOC();
	~TOC() = default;

	BlockType expected_block_type() const override;
	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;

	inline bool entity_exists(const std::string& anEntityName) const {
		return m_map.count(anEntityName) == 1;
	}

	blocknum_t get_blocknum_by_name(const std::string& anEntityName) const;

	void add(const std::string& anEntityName, blocknum_t aBlockNum);

	void drop(const std::string& anEntityName);

private:
	EntityNumMap m_map;
};

}

#endif