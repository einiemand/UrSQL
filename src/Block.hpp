#pragma once
#ifndef BLOCK_HPP
#define BLOCK_HPP
#include "Storable.hpp"
#include <unordered_map>

namespace UrSQL {

enum class BlockType : char {
	TOC_type = 'C',
	table_type = 'T',
	index_type = 'I',
	row_type = 'R',
	free_type = 'F'
};

constexpr size_type defaultBlockSize = 1024;
constexpr size_type defaultPayloadSize = defaultBlockSize - sizeof(BlockType);

class Block {
public:
	Block();
	explicit Block(BlockType aType);
	explicit Block(const Storable& aStorable);
	~Block() = default;

	Block(const Block& rhs);
	Block& operator=(const Block& rhs) noexcept;

	inline BlockType get_type() const {
		return m_type;
	}

	inline void set_type(BlockType aType) {
		m_type = aType;
	}

	inline const char* get_data() const {
		return m_data;
	}

	inline char* get_data() {
		return m_data;
	}
private:
	BlockType m_type;
	char m_data[defaultPayloadSize];
};

class TOC : public Storable {
public:
	using TableNumMap = std::unordered_map<std::string, blocknum_t>;

	TOC() = default;
	~TOC() = default;

	void encode(Block& aBlock) const override;
	void decode(const Block& aBlock) override;

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