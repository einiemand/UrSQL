#include "model/FolderReader.hpp"
#ifdef _WIN32
#include <Windows.h>

#include "model/Storage.hpp"
#elif defined(__linux__) || defined(__APPLE__)
#include <dirent.h>
#include <sys/stat.h>
#endif

namespace UrSQL {

#ifdef _WIN32
class WindowsDirEntIterator {
public:
    explicit WindowsDirEntIterator(const std::string& aDirPath)
        : m_fileHandle(INVALID_HANDLE_VALUE),
          m_fileData() {
        std::string theFileWildCard = aDirPath + "/*";
        m_fileHandle = FindFirstFileA(theFileWildCard.c_str(), &m_fileData);
    }

    ~WindowsDirEntIterator() {
        FindClose(m_fileHandle);
    }

    WindowsDirEntIterator(const WindowsDirEntIterator&) = delete;
    WindowsDirEntIterator& operator=(const WindowsDirEntIterator&) = delete;

    bool next() {
        return FindNextFileA(m_fileHandle, &m_fileData);
    }

    bool isNotDir() const {
        return (*this) &&
               !(m_fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    explicit operator bool() const {
        return m_fileHandle != INVALID_HANDLE_VALUE;
    }

    std::string currentFileName() const {
        return std::string(m_fileData.cFileName);
    }

    static bool directoryExists(const std::string& aDirPath) {
        DWORD theFileAttributes = GetFileAttributesA(aDirPath.c_str());
        return theFileAttributes != INVALID_FILE_ATTRIBUTES &&
               (theFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    static bool createDirectory(const std::string& aDirPath) {
        return CreateDirectoryA(aDirPath.c_str(), nullptr);
    }

private:
    HANDLE m_fileHandle;
    WIN32_FIND_DATAA m_fileData;
};
using DirEntIterator = WindowsDirEntIterator;
#elif defined(__linux__) || defined(__APPLE__)
class UnixDirEntIterator {
public:
    explicit UnixDirEntIterator(const std::string& aDirPath)
        : m_dir(opendir(aDirPath.c_str())),
          m_ent(nullptr) {
        if (m_dir) {
            m_ent = readdir(m_dir);
        }
    }

    ~UnixDirEntIterator() {
        closedir(m_dir);
    }

    UnixDirEntIterator(const UnixDirEntIterator&) = delete;
    UnixDirEntIterator& operator=(const UnixDirEntIterator&) = delete;

    bool next() {
        m_ent = readdir(m_dir);
        return bool(*this);
    }

    bool isNotDir() const {
        return m_ent->d_type != DT_DIR;
    }

    explicit operator bool() const {
        return m_dir && m_ent;
    }

    std::string currentFileName() const {
        return std::string(m_ent->d_name);
    }

    static bool directoryExists(const std::string& aDirPath) {
        struct stat theFileStat;
        stat(aDirPath.c_str(), &theFileStat);
        return S_ISDIR(theFileStat.st_mode);
    }

    static bool createDirectory(const std::string& aDirPath) {
        return mkdir(aDirPath.c_str(), ACCESSPERMS) == 0;
    }

private:
    DIR* m_dir;
    struct dirent* m_ent;
};
using DirEntIterator = UnixDirEntIterator;
#endif

FolderReader::FolderReader(std::string aPath) : m_path(std::move(aPath)) {}

StatusResult FolderReader::eachFile(FileVisitor aVisitor) const {
    StatusResult theResult(Error::no_error);
    if (!DirEntIterator::directoryExists(m_path) &&
        !DirEntIterator::createDirectory(m_path))
    {
        theResult.setError(Error::folder_notExist, '\'' + m_path + '\'');
    }
    if (theResult) {
        DirEntIterator theDirEntIter(m_path);
        if (theDirEntIter) {
            do {
                if (theDirEntIter.isNotDir() &&
                    !aVisitor(theDirEntIter.currentFileName()))
                {
                    break;
                }
            } while (theDirEntIter.next());
        }
    }
    return theResult;
}

}  // namespace UrSQL
