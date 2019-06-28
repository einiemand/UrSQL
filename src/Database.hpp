#pragma once
#ifndef DATABASE_HPP
#define DATABASE_HPP
#include "Storage.hpp"
#include "TOC.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include <memory>
#include <fstream>
#include <unordered_map>

namespace UrSQL {

class Database {
public:
	using EntityCache = std::unordered_map< std::string, std::unique_ptr<Entity> >;

	Database(const std::string& aFileName, CreateNewFile, StatusResult& aResult);
	Database(const std::string& aFileName, OpenExistingFile, StatusResult& aResult);
	~Database();

	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	StatusResult createTable(const AttributeList& anAttributeList, const std::string& anEntityName);
	StatusResult describeTable(const std::string& anEntityName, size_type& theAttributeCount);

	inline const std::string& getName() const {
		return m_storage.getName();
	}

	inline Storage& getStorage() {
		return m_storage;
	}

	inline bool entityExists(const std::string& anEntityName) const {
		return m_toc.entityExists(anEntityName);
	}

	inline bool entityCached(const std::string& anEntityName) const {
		return m_entityCache.count(anEntityName) == 1;
	}

	Entity* getEntityByName(const std::string& anEntityName, StatusResult& aResult);
private:
	TOC m_toc;
	Storage m_storage;
	EntityCache m_entityCache;

	inline void _addEntityToCache(std::unique_ptr<Entity>&& anEntity, const std::string& anEntityName) {
		m_entityCache[anEntityName] = std::move(anEntity);
	}
};

}

#endif /* Database.hpp */