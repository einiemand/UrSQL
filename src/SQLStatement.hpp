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

	static std::unique_ptr<SQLStatement> factory(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter);
protected:
	SQLInterpreter& m_interpreter;
	std::string m_name;

	StatusResult _parseTableName();
};

}


#endif