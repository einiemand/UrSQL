#pragma once

#include "Keyword.hpp"
#include "common/Error.hpp"

namespace ursql {

class Tokenizer;

class Statement {
public:
    explicit Statement(Tokenizer& aTokenizer);
    virtual ~Statement() = default;

    URSQL_DISABLE_COPY(Statement);

    StatusResult run();

    virtual StatusResult parse() = 0;
    virtual StatusResult validate() const = 0;
    virtual StatusResult execute() const = 0;

protected:
    Tokenizer& m_tokenizer;
};

class BasicStatement : public Statement {
public:
    BasicStatement(Tokenizer& aTokenizer, Keyword aKeyword);
    ~BasicStatement() override = default;

    StatusResult parse() override;
    StatusResult validate() const override;
    StatusResult execute() const override;

private:
    Keyword m_keyword;
};

}  // namespace ursql
