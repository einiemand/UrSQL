#pragma once
#ifndef STATEMENT_HPP
#define STATEMENT_HPP
#include "Error.hpp"
#include "Keyword.hpp"

namespace UrSQL {

class Tokenizer;

class Statement {
public:
	Statement(Tokenizer& aTokenizer);

	Statement(const Statement&) = delete;
	Statement& operator=(const Statement&) = delete;

	StatusResult run();

	virtual StatusResult parse() = 0;
	virtual StatusResult validate() const = 0;
	virtual StatusResult execute() const = 0;
	virtual ~Statement() = default;
protected:
	Tokenizer& m_tokenizer;
};

class BasicStatement : public Statement {
public:
	BasicStatement(Tokenizer& aTokenizer, Keyword aKeyword);
	~BasicStatement() = default;

	StatusResult parse() override;
	StatusResult validate() const override;
	StatusResult execute() const override;
private:
	Keyword m_keyword;
};

}

#endif /* Statement.hpp */