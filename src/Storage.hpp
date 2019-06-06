#pragma once
#ifndef STORAGE_HPP
#define STORAGE_HPP
#include <fstream>
#include "Error.hpp"
#include "Block.hpp"

namespace UrSQL {

struct OpenExistingFile {};
struct CreateNewFile {};

class Storage {
public:
	Storage(std::string aFileName, OpenExistingFile);
	Storage(std::string aFileName, CreateNewFile);
	~Storage() = default;

	Storage(const Storage&) = delete;
	Storage& operator=(const Storage&) = delete;

	inline bool ready() const {
		return m_file.is_open();
	}

	std::string get_file_path() const;
	size_type get_block_count() const;

	StatusResult read_block(Block& aBlock, int32_t aBlockNum);
	StatusResult write_block(const Block& aBlock, int32_t aBlockNum);

	static const char* default_storage_path;
	static const char* default_file_extension;
private:
	std::string m_name;
	std::fstream m_file;
};

}

#endif /* Storage.hpp */