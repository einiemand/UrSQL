#pragma once
#ifndef BLOCK_HPP
#define BLOCK_HPP
#include "Util.hpp"
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

class MonoStorable;

class Block {
public:
	explicit Block(BlockType aType = BlockType::free_type);
	explicit Block(const MonoStorable& aMonoStorable);
	~Block() = default;

	Block(const Block& rhs) noexcept;
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

static_assert(sizeof(Block) == defaultBlockSize, "Block size is not equal to defaultBlockSize. Fix it!");

}

#endif