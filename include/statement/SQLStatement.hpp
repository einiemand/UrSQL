#pragma once

#include "Statement.hpp"

namespace ursql {

class SQLStatement : public Statement {
public:
    explicit SQLStatement(std::string tableName);
    ~SQLStatement() override = default;

protected:
    std::string tableName_;
};

}
