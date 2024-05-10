#pragma once

#include "common/Macros.hpp"
#include "parser/TokenEnums.hpp"

namespace ursql {

class Statement {
public:
    explicit Statement() = default;
    virtual ~Statement() = default;

    URSQL_DISABLE_COPY(Statement);

    virtual void validate() const = 0;
    virtual bool execute() const = 0;
};

class BasicStatement : public Statement {
public:
    explicit BasicStatement(Keyword keyword);
    ~BasicStatement() override = default;

    void validate() const override;
    bool execute() const override;

private:
    Keyword keyword_;
};

}  // namespace ursql
