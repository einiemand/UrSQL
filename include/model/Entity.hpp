#pragma once
#ifndef TABLE_HPP
#define TABLE_HPP
#include "Attribute.hpp"
#include <algorithm>

namespace UrSQL {

class Row;

class Entity : public MonoStorable {
public:
	using BlocknumList = std::vector<blocknum_t>;
	using int_t = Value::int_t;

	Entity(blocknum_t aBlocknum);
	~Entity() override = default;

	URSQL_DISABLE_COPY(Entity);

	BlockType expectedBlockType() const override;
	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;

	inline bool isDirty() const {
		return m_dirty;
	}

	inline void makeDirty(bool anIsDirty) {
		m_dirty = anIsDirty;
	}

	void addAttribute(Attribute anAttribute);

	bool attributeExistsByName(const std::string& aName) const;
	const Attribute& getAttributeByName(const std::string& aName) const;

	inline const AttributeList& getAttributes() const {
		return m_attributes;
	}

	int_t getNextAutoincr();

	void addRowPosition(blocknum_t aBlocknum);
	void dropRowPosition(blocknum_t aBlocknum);
	inline const BlocknumList& getRowPos() const {
		return m_rowPos;
	}

	StatusResult generateNewRow(Row& aRow, const StringList& aFieldNames, const StringList& aValueStrs);
private:
	bool m_dirty;
	AttributeList m_attributes;
	size_type m_autoincr;
	BlocknumList m_rowPos;

	bool _blocknumExists(blocknum_t aBlocknum) const;
	size_type _indexOfBlocknum(blocknum_t aBlocknum) const;
};

}

#endif