#pragma once
#ifndef SQLSTATEMENT_HPP
#define SQLSTATEMENT_HPP
#include "Statement.hpp"

namespace UrSQL {

class SQLInterpreter;

class SQLStatement : public Statement {
public:
	SQLStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter);
	~SQLStatement() override = default;

	static std::unique_ptr<SQLStatement> factory(Keyword aStmtType, Tokenizer& aTokenizer, SQLInterpreter& anInterpreter);
private:
	SQLInterpreter& m_interpreter;
protected:
	std::string m_name;

	StatusResult _parseTableName();
};

}


#endif