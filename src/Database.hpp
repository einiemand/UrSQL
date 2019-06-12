#pragma once
#ifndef DATABASE_HPP
#define DATABASE_HPP
#include "Storage.hpp"
#include "TOC.hpp"
#include "Table.hpp"
#include <string>
#include <memory>
#include <fstream>
#include <unordered_map>

namespace UrSQL {

class Database {
public:
	using TableCache = std::unordered_map< std::string, std::unique_ptr<Table> >;

	Database(const std::string& aFileName, CreateNewFile, StatusResult& aResult);
	Database(const std::string& aFileName, OpenExistingFile, StatusResult& aResult);
	~Database() = default;

	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	inline const std::string& get_name() const {
		return m_storage.get_name();
	}
private:
	TOC m_toc;
	Storage m_storage;
	TableCache m_tables;
};

}

#endif /* Database.hpp */