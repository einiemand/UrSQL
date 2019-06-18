#pragma once
#ifndef DATABASE_HPP
#define DATABASE_HPP
#include "Storage.hpp"
#include "TOC.hpp"
#include "Entity.hpp"
#include <memory>
#include <fstream>
#include <unordered_map>

namespace UrSQL {

class Database {
public:
	using EntityCache = std::unordered_map< std::string, std::unique_ptr<Entity> >;

	Database(const std::string& aFileName, CreateNewFile, StatusResult& aResult);
	Database(const std::string& aFileName, OpenExistingFile, StatusResult& aResult);
	~Database() = default;

	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	inline const std::string& getName() const {
		return m_storage.getName();
	}

	inline Storage& getStorage() {
		return m_storage;
	}
private:
	TOC m_toc;
	Storage m_storage;
	EntityCache m_entities;
};

}

#endif /* Database.hpp */