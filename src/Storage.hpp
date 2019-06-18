#pragma once
#ifndef STORAGE_HPP
#define STORAGE_HPP
#include "Error.hpp"
#include <fstream>
#include <set>
#include <functional>

namespace UrSQL {

struct OpenExistingFile {};
struct CreateNewFile {};

class Block;
class TOC;

class Storage {
public:
	using BlockVisitor = std::function<StatusResult(Block&, blocknum_t)>;

	Storage(std::string aFileName, CreateNewFile, const TOC& aTOC, StatusResult& aResult);
	Storage(std::string aFileName, OpenExistingFile, TOC& aTOC, StatusResult& aResult);
	~Storage() = default;

	Storage(const Storage&) = delete;
	Storage& operator=(const Storage&) = delete;

	inline bool storageReady() const {
		return m_file.is_open();
	}

	inline const std::string& getName() const {
		return m_name;
	}

	StatusResult readBlock(Block& aBlock, blocknum_t aBlocknum);
	StatusResult writeBlock(const Block& aBlock, blocknum_t aBlocknum);

	StatusResult eachBlock(BlockVisitor aVisitor);

	static const char* defaultStoragePath;
	static const char* defaultFileExtension;
	static const size_type extensionLength;

	static std::string getDBFilePath(const std::string& aDBName);
	static bool hasDefaultExtension(const std::string& aFileName);
private:
	std::string m_name;
	std::fstream m_file;

	size_type _getBlockCount();
	StatusResult _getFreeBlocknumber(blocknum_t& aFreeBlocknum);

	StatusResult _setupTOC(const TOC& aTOC);
	StatusResult _loadTOC(TOC& aTOC);
};

}

#endif /* Storage.hpp */