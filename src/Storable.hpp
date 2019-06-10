#pragma once
#ifndef STORABLE_HPP
#define STORABLE_HPP
#include "Error.hpp"

namespace UrSQL {

class Block;
using blocknum_t = int32_t;

class Storable {
public:
	Storable(blocknum_t aBlockNum = -1) : m_blocknum(aBlockNum)
	{
	}

	Storable(const Storable&) = delete;
	Storable& operator=(const Storable&) = delete;

	virtual void encode(Block& aBlock) const = 0;
	virtual void decode(const Block& aBlock, blocknum_t aBlockNum) = 0;
	virtual ~Storable() = 0 {}

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
