#pragma once
#ifndef TABLE_HPP
#define TABLE_HPP
#include "Storable.hpp"

namespace UrSQL {

class Entity : public MonoStorable {
public:
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	BlockType expectedBlockType() const override;
	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;
private:

};

}

#endif