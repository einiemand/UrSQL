#include "FolderReader.hpp"
#ifdef _WIN32
#include "Storage.hpp"
#include <Windows.h>
#elif defined(__LINUX__) || defined(__APPLE__)

#endif

namespace UrSQL {

#ifdef _WIN32
class DirEntIterator {
public:
	explicit DirEntIterator(const std::string& aDirPath) :
		m_fileHandle(INVALID_HANDLE_VALUE),
		m_fileData()
	{
		std::string theFileWildCard = aDirPath + "/*";
		m_fileHandle = FindFirstFileA(theFileWildCard.c_str(), &m_fileData);
	}

	~DirEntIterator() {
		FindClose(m_fileHandle);
	}

	DirEntIterator(const DirEntIterator&) = delete;
	DirEntIterator& operator=(const DirEntIterator&) = delete;

	bool next() {
		return FindNextFileA(m_fileHandle, &m_fileData);
	}

	bool isNotDir() const {
		return (*this) && !(m_fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	explicit operator bool() const {
		return m_fileHandle != INVALID_HANDLE_VALUE && m_fileData.dwFileAttributes != INVALID_FILE_ATTRIBUTES;
	}

	std::string currentFileName() const {
		return std::string(m_fileData.cFileName);
	}

	static bool directoryExists(const std::string& aDirPath) {
		DWORD theFileAttributes = GetFileAttributesA(aDirPath.c_str());
		return theFileAttributes != INVALID_FILE_ATTRIBUTES && (theFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	static bool createDirectory(const std::string& aDirPath) {
		return CreateDirectoryA(aDirPath.c_str(), nullptr);
	}
private:
	HANDLE m_fileHandle;
	WIN32_FIND_DATAA m_fileData;
};
#elif defined(__linux__) || defined(__APPLE__)

#endif

FolderReader::FolderReader(std::string aPath) :
	m_path(std::move(aPath))
{
}

StatusResult FolderReader::eachFile(FileVisitor aVisitor) const {
	StatusResult theResult(Error::no_error);
	if (!DirEntIterator::directoryExists(m_path)
		&& !DirEntIterator::createDirectory(m_path))
	{
		theResult.setError(Error::folder_notExist, '\'' + m_path + '\'');
	}
	if (theResult) {
		DirEntIterator theDirEntIter(m_path);
		if (theDirEntIter) {
			do {
				if (theDirEntIter.isNotDir()
					&& !aVisitor(theDirEntIter.currentFileName()))
				{
					break;
				}
			} while (theDirEntIter.next());
		}
	}
	return theResult;
}

}