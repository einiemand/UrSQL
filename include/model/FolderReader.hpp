#pragma once

#include "common/Error.hpp"
#include <functional>

namespace UrSQL {

class FolderReader {
public:
	using FileVisitor = std::function<bool(const std::string&)>;

	explicit FolderReader(std::string aPath);
	~FolderReader() = default;

	URSQL_DISABLE_COPY(FolderReader);

	StatusResult eachFile(FileVisitor aVisitor) const;
private:
	std::string m_path;
};

}
