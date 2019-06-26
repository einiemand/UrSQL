#pragma once
#ifndef TABLE_HPP
#define TABLE_HPP
#include "Attribute.hpp"
#include <algorithm>

namespace UrSQL {

class Entity : public MonoStorable {
public:
	Entity() = default;
	~Entity() override = default;

	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	inline void addAttribute(const Attribute& anAttribute) {
		m_attributes.emplace_back(anAttribute);
	}

	bool attributeExistsByName(const std::string& aName) const;
	const Attribute& getAttributeByName(const std::string& aName) const;

	inline const AttributeList& getAttributes() const {
		return m_attributes;
	}

	BlockType expectedBlockType() const override;
	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;
private:
	AttributeList m_attributes;
};

}

#endif