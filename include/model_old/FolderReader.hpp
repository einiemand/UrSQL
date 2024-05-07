#pragma once

#include <functional>

#include "common/Error.hpp"

namespace ursql {

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

}  // namespace ursql
