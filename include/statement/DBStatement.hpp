#pragma once

#include <memory>
#include <string>

#include "Statement.hpp"
#include "parser/TokenStream.hpp"

namespace ursql {

class DBManager;

class SingleDBStatement : public Statement {
public:
    explicit SingleDBStatement(std::string dbName);
    ~SingleDBStatement() override = default;

protected:
    const std::string dbName_;
};

class MultiDBStatement : public Statement {
public:
    explicit MultiDBStatement(std::vector<std::string> dbNames);
    ~MultiDBStatement() override = default;

protected:
    const std::vector<std::string> dbNames_;
};

class CreateDBStatement : public MultiDBStatement {
public:
    explicit CreateDBStatement(std::vector<std::string> dbNames);
    ~CreateDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;

    static std::unique_ptr<CreateDBStatement> parse(TokenStream& ts);
};

class DropDBStatement : public MultiDBStatement {
public:
    explicit DropDBStatement(std::vector<std::string> dbNames);
    ~DropDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;

    static std::unique_ptr<DropDBStatement> parse(TokenStream& ts);
};

class UseDBStatement : public SingleDBStatement {
public:
    explicit UseDBStatement(std::string dbName);
    ~UseDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;

    static std::unique_ptr<UseDBStatement> parse(TokenStream& ts);
};

class DescDBStatement : public SingleDBStatement {
public:
    explicit DescDBStatement(std::string dbName);
    ~DescDBStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;

    static std::unique_ptr<DescDBStatement> parse(TokenStream& ts);
};

}  // namespace ursql
