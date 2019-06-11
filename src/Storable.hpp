#pragma once
#ifndef STORABLE_HPP
#define STORABLE_HPP
#include "Error.hpp"
#include "Block.hpp"

namespace UrSQL {

class BufferWriter;
class BufferReader;

class Storable {
public:
	Storable() = default;

	Storable(const Storable&) = delete;
	Storable& operator=(const Storable&) = delete;

	virtual void serialize(BufferWriter& aWriter) const = 0;
	virtual void deserialize(BufferReader& aReader) = 0;
	virtual ~Storable() = default;
};


class MonoStorable : public Storable {
public:
	MonoStorable(blocknum_t aBlockNum = -1);
	~MonoStorable() override = default;

	virtual BlockType expected_block_type() const = 0;

	void encode(Block& aBlock) const;
	void decode(const Block& aBlock, blocknum_t aBlockNum);

	inline blocknum_t get_blocknum() const {
		return m_blocknum;
	}

	inline void set_blocknum(blocknum_t aBlockNum) {
		m_blocknum = aBlockNum;
	}

protected:
	blocknum_t m_blocknum;
};

}
#endif /* Storable.hpp */
