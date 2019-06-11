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

	Storage(std::string aFileName, CreateNewFile);
	Storage(std::string aFileName, OpenExistingFile);
	~Storage() = default;

	Storage(const Storage&) = delete;
	Storage& operator=(const Storage&) = delete;

	inline bool storage_ready() const {
		return m_file.is_open();
	}

	std::string get_file_path() const;
	size_type get_block_count();
	StatusResult get_free_block_number(blocknum_t& aFreeBlockNum);

	StatusResult setup_toc(const TOC& aTOC);
	StatusResult load_toc(TOC& aTOC);

	StatusResult read_block(Block& aBlock, blocknum_t aBlockNum);
	StatusResult write_block(const Block& aBlock, blocknum_t aBlockNum);

	StatusResult each_block(BlockVisitor aVisitor);

	static const char* default_storage_path;
	static const char* default_file_extension;
private:
	std::string m_name;
	std::fstream m_file;
};

}

#endif /* Storage.hpp */