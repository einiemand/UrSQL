#include "controller/DBManager.hpp"

#include <format>

#include "exception/InternalError.hpp"
#include "exception/UserError.hpp"

namespace ursql {

DBManager::DBManager(const fs::path& dbDirectoryPath, fs::path dbFileExtension)
    : dbDirectoryPath_(fs::weakly_canonical(dbDirectoryPath)),
      dbFileExtension_(std::move(dbFileExtension)) {}

Database* DBManager::getActiveDB() {
    return activeDB_.get();
}

std::unique_ptr<Database> DBManager::getExistingDBByName(
  const std::string& dbName) {
    URSQL_EXPECT(databaseExists(dbName), DoesNotExist, dbName);
    return std::make_unique<Database>(dbName, _dbName2Path(dbName),
                                      OpenExistingFile{});
}

bool DBManager::databaseExists(const std::string& dbName) {
    return fs::exists(_dbName2Path(dbName));
}

void DBManager::createDatabases(const std::vector<std::string>& dbNames) {
    std::ranges::for_each(dbNames, [this](auto&& dbName) {
        URSQL_EXPECT(!databaseExists(dbName), AlreadyExists, dbName);
    });
    std::ranges::for_each(dbNames, [this](auto&& dbName) {
        Database(dbName, _dbName2Path(dbName), CreateNewFile{});
    });
}

void DBManager::dropDatabases(const std::vector<std::string>& dbNames) {
    std::ranges::for_each(dbNames, [this](auto&& dbName) {
        URSQL_EXPECT(databaseExists(dbName), DoesNotExist, dbName);
    });
    std::ranges::for_each(dbNames, [this](auto&& dbName) {
        URSQL_EXPECT(fs::remove(_dbName2Path(dbName)), FileAccessError,
                     std::format("unable to drop db {}", dbName));
    });
}

void DBManager::useDatabase(const std::string& dbName) {
    if (!activeDB_ || activeDB_->getName() != dbName) {
        activeDB_ = getExistingDBByName(dbName);
    }
}

std::vector<std::string> DBManager::getDatabaseNames() {
    std::vector<std::string> dbNames;
    std::ranges::for_each(
      fs::directory_iterator(dbDirectoryPath_), [&](auto& entry) {
          std::optional<std::string> dbNameOpt = _dirEnt2DbName(entry);
          if (dbNameOpt.has_value()) {
              dbNames.push_back(std::move(dbNameOpt.value()));
          }
      });
    return dbNames;
}

fs::path DBManager::_dbName2Path(const std::string& dbName) {
    return (dbDirectoryPath_ / dbName).replace_extension(dbFileExtension_);
}

std::optional<std::string> DBManager::_dirEnt2DbName(
  const fs::directory_entry& entry) {
    auto& path = entry.path();
    URSQL_ASSERT(
      path.parent_path() == dbDirectoryPath_,
      std::format("db file parent path should match. Expected={}, Actual={}",
                  dbDirectoryPath_.native(), path.parent_path().native()));
    if (!entry.is_regular_file() || path.extension() != dbFileExtension_ ||
        !path.has_stem())
    {
        return std::nullopt;
    }
    return path.stem();
}

}  // namespace ursql
