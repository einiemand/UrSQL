#pragma once

#include "ExecuteResult.hpp"
#include "common/Macros.hpp"
#include "parser/TokenEnums.hpp"

namespace ursql {

class DBManager;

class Statement {
public:
    explicit Statement() = default;
    virtual ~Statement() = default;

    URSQL_DISABLE_COPY(Statement);

    [[nodiscard]] virtual ExecuteResult run(DBManager& dbManager) const = 0;
};

class NopStatement : public Statement {
public:
    explicit NopStatement() = default;
    ~NopStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager&) const override;
};

class HelpStatement : public Statement {
public:
    explicit HelpStatement() = default;
    ~HelpStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager&) const override;
};

class VersionStatement : public Statement {
public:
    explicit VersionStatement() = default;
    ~VersionStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager&) const override;
};

class QuitStatement : public Statement {
public:
    explicit QuitStatement() = default;
    ~QuitStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager&) const override;
};

}  // namespace ursql
