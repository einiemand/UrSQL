#pragma once
#ifndef STORABLE_HPP
#define STORABLE_HPP
#include "../Error.hpp"
#include "Block.hpp"

namespace UrSQL {

class BufferWriter;
class BufferReader;

class Storable {
public:
	Storable() = default;

	virtual void serialize(BufferWriter& aWriter) const = 0;
	virtual void deserialize(BufferReader& aReader) = 0;
	virtual ~Storable() = default;
};


class MonoStorable : public Storable {
public:
	MonoStorable(blocknum_t aBlocknum);
	~MonoStorable() override = default;

	virtual BlockType expectedBlockType() const = 0;

	void encode(Block& aBlock) const;
	void decode(const Block& aBlock);

	inline blocknum_t getBlocknum() const {
		return m_blocknum;
	}

	inline void setBlocknum(blocknum_t aBlocknum) {
		m_blocknum = aBlocknum;
	}

protected:
	blocknum_t m_blocknum;
};

}
#endif /* Storable.hpp */
