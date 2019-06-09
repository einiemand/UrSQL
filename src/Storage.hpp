#pragma once
#ifndef STORAGE_HPP
#define STORAGE_HPP
#include <fstream>
#include "Error.hpp"
#include "Block.hpp"
#include <set>

namespace UrSQL {

struct OpenExistingFile {};
struct CreateNewFile {};

class Storage {
public:
	using FreeBlockNumSet = std::set<blocknum_t>;

	Storage(std::string aFileName, CreateNewFile);
	Storage(std::string aFileName, OpenExistingFile);
	~Storage() = default;

	Storage(const Storage&) = delete;
	Storage& operator=(const Storage&) = delete;

	inline bool ready() const {
		return m_file.is_open();
	}

	std::string get_file_path() const;
	size_type get_block_count();
	blocknum_t get_free_block_number();

	StatusResult setup_toc(const TOC& aTOC);
	StatusResult load_toc(TOC& aTOC);

	StatusResult read_block(Block& aBlock, blocknum_t aBlockNum);
	StatusResult write_block(const Block& aBlock, blocknum_t aBlockNum);

	static const char* default_storage_path;
	static const char* default_file_extension;
private:
	std::string m_name;
	std::fstream m_file;
	FreeBlockNumSet m_set;
};

}

#endif /* Storage.hpp */