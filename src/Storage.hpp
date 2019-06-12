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

	inline bool storage_ready() const {
		return m_file.is_open();
	}

	inline const std::string& get_name() const {
		return m_name;
	}

	StatusResult read_block(Block& aBlock, blocknum_t aBlockNum);
	StatusResult write_block(const Block& aBlock, blocknum_t aBlockNum);

	StatusResult each_block(BlockVisitor aVisitor);

	static const char* default_storage_path;
	static const char* default_file_extension;

	static std::string get_dbfile_path(const std::string& aDBName);
private:
	std::string m_name;
	std::fstream m_file;

	size_type _get_block_count();
	StatusResult _get_free_block_number(blocknum_t& aFreeBlockNum);

	StatusResult _setup_toc(const TOC& aTOC);
	StatusResult _load_toc(TOC& aTOC);
};

}

#endif /* Storage.hpp */