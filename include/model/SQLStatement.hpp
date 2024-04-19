#pragma once

#include <memory>

#include "Statement.hpp"

namespace ursql {

class SQLInterpreter;

class SQLStatement : public Statement {
public:
    SQLStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter);
    ~SQLStatement() override = default;

    static std::unique_ptr<SQLStatement> factory(Tokenizer& aTokenizer,
                                                 SQLInterpreter& anInterpreter);

protected:
    SQLInterpreter& m_interpreter;
    std::string m_entityName;

    StatusResult _parseTableName();
};

}  // namespace ursql
