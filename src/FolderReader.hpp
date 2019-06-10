#pragma once
#ifndef FOLDERREADER_HPP
#define FOLDERREADER_HPP
#include "Error.hpp"
#include <string>
#include <functional>

namespace UrSQL {

class FolderReader {
public:
	using FileVisitor = std::function<void(const std::string&)>;

	FolderReader(std::string aPath);
	~FolderReader() = default;

	FolderReader(const FolderReader&) = delete;
	FolderReader& operator=(const FolderReader&) = delete;

	StatusResult each_file(FileVisitor aVisitor) const;
private:
	std::string m_path;
};

}


#endif /* FOLDERREADER_HPP */
