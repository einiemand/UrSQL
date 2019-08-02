#pragma once
#ifndef FOLDERREADER_HPP
#define FOLDERREADER_HPP
#include "common/Error.hpp"
#include <functional>

namespace UrSQL {

class FolderReader {
public:
	using FileVisitor = std::function<bool(const std::string&)>;

	FolderReader(std::string aPath);
	~FolderReader() = default;

	FolderReader(const FolderReader&) = delete;
	FolderReader& operator=(const FolderReader&) = delete;

	StatusResult eachFile(FileVisitor aVisitor) const;
private:
	std::string m_path;
};

}


#endif /* FOLDERREADER_HPP */
