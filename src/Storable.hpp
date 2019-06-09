#pragma once
#ifndef STORABLE_HPP
#define STORABLE_HPP
#include "Error.hpp"

namespace UrSQL {

class Block;

class Storable {
public:
	Storable() = default;

	Storable(const Storable&) = delete;
	Storable& operator=(const Storable&) = delete;

	virtual StatusResult encode(Block& aBlock) const = 0;
	virtual StatusResult decode(const Block& aBlock) = 0;
	virtual ~Storable() = 0 {}
};

}
#endif /* Storable.hpp */
