#pragma once

#include "model/Database.hpp"

namespace ursql {

class DBManager {
public:
    explicit DBManager() = default;
    ~DBManager() = default;

    URSQL_DISABLE_COPY(DBManager);

    static bool databaseExists(std::string_view dbName);

    void createDatabase(std::string_view dbName);
    void dropDatabase(std::string_view dbName);
    void useDatabase(std::string_view dbName);

private:
    std::unique_ptr<Database> activeDB_;
};

}

