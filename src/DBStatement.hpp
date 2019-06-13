#pragma once
#ifndef DBSTATEMENT_HPP
#define DBSTATEMENT_HPP
#include "Statement.hpp"

namespace UrSQL {

class DBManager;

class DBStatement : public Statement {
public:
	DBStatement(Tokenizer& aTokenizer, DBManager& aDBManager) :
		Statement(aTokenizer),
		m_manager(aDBManager)
	{
	}

	~DBStatement() override = default;
protected:
	DBManager& m_manager;
};

/* -------------------------------CreateDBStatement------------------------------- */
class CreateDBStatement : public DBStatement {
public:
	CreateDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager);
	~CreateDBStatement() override = default;

	StatusResult parse() override;
	StatusResult validate() const override;
	StatusResult run() const override;
private:
	std::string m_dbname;
};

/* -------------------------------DropDBStatement------------------------------- */
class DropDBStatement : public DBStatement {
public:
	DropDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager);
	~DropDBStatement() override = default;

	StatusResult parse() override;
	StatusResult validate() const override;
	StatusResult run() const override;
private:
	std::string m_dbname;
};

/* -------------------------------UseDBStatement------------------------------- */
class UseDBStatement : public DBStatement {
public:
	UseDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager);
	~UseDBStatement() override = default;

	StatusResult parse() override;
	StatusResult validate() const override;
	StatusResult run() const override;
private:
	std::string m_dbname;
};

class ShowDBStatement : public DBStatement {
public:
	ShowDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager);
	~ShowDBStatement() override = default;

	StatusResult parse() override;
	StatusResult validate() const override;
	StatusResult run() const override;
};

}

#endif
