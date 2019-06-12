#pragma once
#ifndef DBMANAGER_HPP
#define DBMANAGER_HPP
#include "Interpreter.hpp"

namespace UrSQL {

class Statement;
class Database;

class DBManager : public Interpreter {
public:
	DBManager(Interpreter* anInterpreter = nullptr);
	~DBManager() = default;

	std::unique_ptr<Statement> get_statement(Tokenizer& aTokenizer) override;

	StatusResult create_database(const std::string& aName);
	StatusResult drop_database(const std::string& aName);
	StatusResult use_database(const std::string& aName);
	StatusResult show_databases();
	StatusResult describe_database(const std::string& aName);

	Database* get_active_database() const override;

	static StatusResult database_exists(bool& exists, const std::string& aName);

private:
	std::unique_ptr<Database> m_activeDB;

	void _reset_active_db(std::unique_ptr<Database>&& aNewDB);
	void _release_active_db();

	static StatusResult _delete_dbfile(const std::string& aName);
};

}

#endif /* DBManager.hpp */