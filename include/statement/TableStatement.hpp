#pragma once

#include <vector>

#include "Statement.hpp"

namespace ursql {

class SingleTableStatement : public Statement {
public:
    explicit SingleTableStatement(std::string tableName);
    ~SingleTableStatement() override = default;

protected:
    const std::string tableName_;
};

class MultiTableStatement : public Statement {
public:
    explicit MultiTableStatement(std::vector<std::string> tableNames);
    ~MultiTableStatement() override = default;

protected:
    const std::vector<std::string> tableNames_;
};

}  // namespace ursql
