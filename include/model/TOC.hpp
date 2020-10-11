#pragma once
#ifndef TOC_HPP
#define TOC_HPP
#include "Storable.hpp"

namespace UrSQL {

class TOC : public MonoStorable {
public:
	using EntityMap = std::unordered_map<std::string, blocknum_t>;

	TOC();
	~TOC() override = default;

	URSQL_DISABLE_COPY(TOC);

	BlockType expectedBlockType() const override;
	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;

	inline bool isDirty() const {
		return m_dirty;
	}

	inline void makeDirty(bool anIsDirty) {
		m_dirty = anIsDirty;
	}

	inline bool entityExists(const std::string& anEntityName) const {
		return m_entityMap.count(anEntityName) == 1;
	}

	blocknum_t getEntityPosByName(const std::string& anEntityName) const;

	void add(const std::string& anEntityName, blocknum_t aBlocknum);
	void drop(const std::string& anEntityName);

	StringList collectEntityNames() const;

private:
	bool m_dirty;
	EntityMap m_entityMap;
};

}

#endif