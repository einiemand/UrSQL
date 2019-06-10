#include "FolderReader.hpp"
#include <filesystem>

namespace UrSQL {



FolderReader::FolderReader(std::string aPath) :
	m_path(std::move(aPath))
{
}

StatusResult FolderReader::each_file(FileVisitor aVisitor) const {
	if (std::filesystem::exists(m_path)) {
		for (const auto& theEntry : std::filesystem::directory_iterator(m_path)) {
			std::string theFilePath = theEntry.path().generic_string();
			aVisitor(theFilePath);
		}
		return StatusResult(Error::no_error);
	}
	return StatusResult(Error::folder_notExist, "Folder " + m_path + " doesn't exist");
}


}