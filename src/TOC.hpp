#pragma once
#ifndef TOC_HPP
#define TOC_HPP
#include "Storable.hpp"

namespace UrSQL {

class TOC : public MonoStorable {
public:
	using EntityMap = std::unordered_map<std::string, blocknum_t>;

	TOC();
	~TOC() = default;

	TOC(const TOC&) = delete;
	TOC& operator=(const TOC&) = delete;

	BlockType expectedBlockType() const override;
	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;

	inline bool entityExists(const std::string& anEntityName) const {
		return m_entityMap.count(anEntityName) == 1;
	}

	blocknum_t getEntityPosByName(const std::string& anEntityName) const;

	void add(const std::string& anEntityName, blocknum_t aBlocknum);

	void drop(const std::string& anEntityName);

private:
	EntityMap m_entityMap;
};

}

#endif