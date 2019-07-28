#include "FolderReader.hpp"
#ifdef _WIN32
#include "Storage.hpp"
#include <Windows.h>
#elif defined(__LINUX__) || defined(__APPLE__)

#endif

namespace UrSQL {

bool directoryExists(const std::string& aDirPath) {
	DWORD theFileAttributes = GetFileAttributesA(aDirPath.c_str());
	return theFileAttributes != INVALID_FILE_ATTRIBUTES && (theFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool isNotDir(const WIN32_FIND_DATAA& aFindData) {
	return aFindData.dwFileAttributes != INVALID_FILE_ATTRIBUTES
		&& !(aFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

FolderReader::FolderReader(std::string aPath) :
	m_path(std::move(aPath))
{
}

StatusResult FolderReader::eachFile(FileVisitor aVisitor) const {
#ifdef _WIN32
	StatusResult theResult(Error::no_error);
	if (!directoryExists(m_path)) {
		if (!CreateDirectoryA(m_path.c_str(), nullptr)) {
			theResult.setError(Error::folder_notExist, '\'' + m_path + '\'');
		}
	}
	WIN32_FIND_DATAA theFindData;
	std::string theFileWildcard = m_path + "/*" + Storage::defaultFileExtension;
	HANDLE theFileHandle = FindFirstFileA(theFileWildcard.c_str(), &theFindData);
	if (theFileHandle != INVALID_HANDLE_VALUE) {
		do {
			if (isNotDir(theFindData)
				&& !aVisitor(theFindData.cFileName))
			{
				break;
			}
		} while (FindNextFileA(theFileHandle, &theFindData));
		FindClose(theFileHandle);
	}
	return theResult;
#elif defined(__LINUX__) || defined(__APPLE__)

#endif
}


}