#pragma once

#include <memory>

#include "Statement.hpp"

namespace UrSQL {

class DBManager;

class DBStatement : public Statement {
public:
    DBStatement(Tokenizer& aTokenizer, DBManager& aDBManager);
    ~DBStatement() override = default;

    static std::unique_ptr<DBStatement> factory(Tokenizer& aTokenizer,
                                                DBManager& aDBManager);

protected:
    DBManager& m_manager;
};

}  // namespace UrSQL
