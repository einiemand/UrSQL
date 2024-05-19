#include "controller/DBManager.hpp"
#include "exception/UserError.hpp"
#include "exception/InternalError.hpp"
#include <format>

namespace ursql {

namespace {

const fs::path directoryPath = fs::temp_directory_path();
const fs::path fileExtension = ".db";

fs::path dbName2Path(std::string_view dbName) {
    return (directoryPath/dbName).replace_extension(fileExtension);
}

}

bool DBManager::databaseExists(std::string_view dbName) {
    return fs::exists(dbName2Path(dbName));
}

void DBManager::createDatabase(std::string_view dbName) {
    URSQL_EXPECT(!databaseExists(dbName), AlreadyExists, dbName);
    activeDB_ = std::make_unique<Database>(std::string(dbName), dbName2Path(dbName), CreateNewFile{});
}

void DBManager::dropDatabase(std::string_view dbName) {
    URSQL_EXPECT(databaseExists(dbName), DoesNotExist, dbName);
    if (activeDB_ && activeDB_->getName() == dbName) {
        activeDB_.reset();
    }
    URSQL_EXPECT(fs::remove(dbName2Path(dbName)), FileAccessError, std::format("unable to drop db {}", dbName));
}

void DBManager::useDatabase(std::string_view dbName) {
    URSQL_EXPECT(databaseExists(dbName), DoesNotExist, dbName);
    if (!activeDB_ || activeDB_->getName() != dbName) {
        activeDB_ = std::make_unique<Database>(std::string(dbName), dbName2Path(dbName), OpenExistingFile{});
    }
}

}
