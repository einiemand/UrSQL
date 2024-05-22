#include "controller/DBManager.hpp"

#include <format>

#include "exception/InternalError.hpp"
#include "exception/UserError.hpp"

namespace ursql {

namespace {

const fs::path directoryPath = fs::weakly_canonical(fs::temp_directory_path());
const fs::path fileExtension = ".db";

fs::path dbName2Path(std::string_view dbName) {
    return (directoryPath / dbName).replace_extension(fileExtension);
}

std::optional<std::string> dirEnt2DbName(const fs::directory_entry& entry) {
    auto& path = entry.path();
    URSQL_ASSERT(
      path.parent_path() == directoryPath,
      std::format("db file parent path should match. Expected={}, Actual={}",
                  directoryPath.native(), path.parent_path().native()));
    if (!entry.is_regular_file() || path.extension() != fileExtension ||
        !path.has_stem())
    {
        return std::nullopt;
    }
    return path.stem();
}

}  // namespace

Database* DBManager::getActiveDB() {
    return activeDB_.get();
}

bool DBManager::databaseExists(std::string_view dbName) {
    return fs::exists(dbName2Path(dbName));
}

void DBManager::createDatabase(std::string_view dbName) {
    URSQL_EXPECT(!databaseExists(dbName), AlreadyExists, dbName);
    activeDB_ = std::make_unique<Database>(
      std::string(dbName), dbName2Path(dbName), CreateNewFile{});
}

void DBManager::dropDatabase(std::string_view dbName) {
    URSQL_EXPECT(databaseExists(dbName), DoesNotExist, dbName);
    if (activeDB_ && activeDB_->getName() == dbName) {
        activeDB_.reset();
    }
    URSQL_EXPECT(fs::remove(dbName2Path(dbName)), FileAccessError,
                 std::format("unable to drop db {}", dbName));
}

void DBManager::useDatabase(std::string_view dbName) {
    URSQL_EXPECT(databaseExists(dbName), DoesNotExist, dbName);
    if (!activeDB_ || activeDB_->getName() != dbName) {
        activeDB_ = std::make_unique<Database>(
          std::string(dbName), dbName2Path(dbName), OpenExistingFile{});
    }
}

std::vector<std::string> DBManager::getDatabaseNames() {
    std::vector<std::string> dbNames;
    std::ranges::for_each(
      fs::directory_iterator(directoryPath), [&dbNames](auto& entry) {
          std::optional<std::string> dbNameOpt = dirEnt2DbName(entry);
          if (dbNameOpt.has_value()) {
              dbNames.push_back(std::move(dbNameOpt.value()));
          }
      });
    return dbNames;
}

}  // namespace ursql
