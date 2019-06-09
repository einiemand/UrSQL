#pragma once
#ifndef STORABLE_HPP
#define STORABLE_HPP
#include "Error.hpp"

namespace UrSQL {

class Block;
using blocknum_t = int32_t;

class Storable {
public:
	Storable() = default;

	Storable(const Storable&) = delete;
	Storable& operator=(const Storable&) = delete;

	virtual void encode(Block& aBlock) const = 0;
	virtual void decode(const Block& aBlock) = 0;
	virtual ~Storable() = 0 {}

	inline blocknum_t get_blocknum() const {
		return m_blocknum;
	}

protected:
	blocknum_t m_blocknum;
};

}
#endif /* Storable.hpp */
