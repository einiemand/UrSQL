#pragma once
#ifndef TABLE_HPP
#define TABLE_HPP
#include "Storable.hpp"

namespace UrSQL {

class Entity : public MonoStorable {
public:
	BlockType expected_block_type() const;
	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;
private:

};

}

#endif