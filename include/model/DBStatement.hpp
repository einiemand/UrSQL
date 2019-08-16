#pragma once
#ifndef DBSTATEMENT_HPP
#define DBSTATEMENT_HPP
#include "Statement.hpp"
#include <memory>

namespace UrSQL {

class DBManager;

class DBStatement : public Statement {
public:
	DBStatement(Tokenizer& aTokenizer, DBManager& aDBManager);
	~DBStatement() override = default;

	static std::unique_ptr<DBStatement> factory(Tokenizer& aTokenizer, DBManager& aDBManager);
protected:
	DBManager& m_manager;
};

}

#endif /* DBStatement.hpp */