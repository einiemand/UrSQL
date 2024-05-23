#pragma once

#include <memory>
#include <string>

#include "Statement.hpp"
#include "parser/TokenStream.hpp"

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

    static std::unique_ptr<CreateDBStatement> parse(TokenStream& ts);
};

class DropDBStatement : public DBStatement {
public:
    explicit DropDBStatement(std::string dbName);
    ~DropDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;

    static std::unique_ptr<DropDBStatement> parse(TokenStream& ts);
};

class UseDBStatement : public DBStatement {
public:
    explicit UseDBStatement(std::string dbName);
    ~UseDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;

    static std::unique_ptr<UseDBStatement> parse(TokenStream& ts);
};

class DescDBStatement : public DBStatement {
public:
    explicit DescDBStatement(std::string dbName);
    ~DescDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;

    static std::unique_ptr<DescDBStatement> parse(TokenStream& ts);
};

}  // namespace ursql
