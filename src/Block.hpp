#pragma once
#ifndef BLOCK_HPP
#define BLOCK_HPP
#include "Error.hpp"
#include <unordered_map>

namespace UrSQL {

class Block;

class Storable {
public:
	Storable(const Storable&) = delete;
	Storable& operator=(const Storable&) = delete;

	virtual void encode(Block& aBlock) const = 0;
	virtual ~Storable() = 0 {}
};

enum class BlockType : char {
	TOC_type = 'C',
	table_type = 'T',
	index_type = 'I',
	row_type = 'R',
	free_type = 'F'
};

constexpr size_type defaultBlockSize = 1024;
constexpr size_type defaultPayloadSize = defaultBlockSize - sizeof(BlockType);

using TableNumMap = std::unordered_map<std::string, int32_t>;

class TOC : public Storable {
public:
	TOC() = default;
	~TOC() = default;

	TOC(const TOC&) = delete;
	TOC& operator=(const TOC&) = delete;

	void encode(Block& aBlock) const override;
	StatusResult decode(const Block& aBlock);

	inline bool table_exists(const std::string& aTableName) const {
		return m_map.count(aTableName) == 1;
	}

	int32_t get_blocknum_by_name(const std::string& aTableName) const;

	StatusResult add(const std::string& aTableName, int32_t aBlockNum);

	StatusResult drop(const std::string& aTableName);

private:
	TableNumMap m_map;
};

class Block {
public:
	Block();
	Block(BlockType aType);
	Block(Storable& aStorable);
	~Block() = default;

	Block(const Block& rhs);
	Block& operator=(const Block& rhs) noexcept;

	inline BlockType get_type() const {
		return m_type;
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

}

#endif