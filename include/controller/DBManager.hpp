#pragma once

#include "model/Database.hpp"

namespace ursql {

class DBManager {
public:
    DBManager(const fs::path& dbDirectoryPath, fs::path dbFileExtension);
    ~DBManager() = default;

    URSQL_DISABLE_COPY(DBManager);

    Database* getActiveDB();
    std::unique_ptr<Database> getExistingDBByName(const std::string& dbName);

    bool databaseExists(const std::string& dbName);
    void createDatabases(const std::vector<std::string>& dbNames);
    void dropDatabases(const std::vector<std::string>& dbNames);
    void useDatabase(const std::string& dbName);
    std::vector<std::string> getDatabaseNames();

private:
    const fs::path dbDirectoryPath_;
    const fs::path dbFileExtension_;
    std::unique_ptr<Database> activeDB_;

    fs::path _dbName2Path(const std::string& dbName);
    std::optional<std::string> _dirEnt2DbName(const fs::directory_entry& entry);
};

}  // namespace ursql
