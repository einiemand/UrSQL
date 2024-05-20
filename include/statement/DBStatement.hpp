#pragma once

#include <string>

#include "Statement.hpp"

namespace ursql {

class DBManager;

class DBStatement : public Statement {
public:
    explicit DBStatement(std::string dbName);
    ~DBStatement() override = default;

protected:
    const std::string dbName_;
};

class CreateDBStatement : public DBStatement {
public:
    explicit CreateDBStatement(std::string dbName);
    ~CreateDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;
};

class DropDBStatement : public DBStatement {
public:
    explicit DropDBStatement(std::string dbName);
    ~DropDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;
};

class UseDBStatement : public DBStatement {
public:
    explicit UseDBStatement(std::string dbName);
    ~UseDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;
};

class ShowDBStatement : public Statement {
public:
    explicit ShowDBStatement() = default;
    ~ShowDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager&) const override;
};

class TokenStream;

namespace parser {

std::unique_ptr<CreateDBStatement> parseCreateDBStatement(TokenStream& ts);
std::unique_ptr<DropDBStatement> parseDropDBStatement(TokenStream& ts);
std::unique_ptr<UseDBStatement> parseUseDBStatement(TokenStream& ts);
std::unique_ptr<ShowDBStatement> parseShowDBStatement(TokenStream& ts);

}  // namespace parser

}  // namespace ursql
