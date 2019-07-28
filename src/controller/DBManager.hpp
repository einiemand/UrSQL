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

	std::unique_ptr<Statement> getStatement(Tokenizer& aTokenizer) override;

	StatusResult createDatabase(const std::string& aName);
	StatusResult dropDatabase(const std::string& aName);
	StatusResult useDatabase(const std::string& aName);
	StatusResult showDatabases();
	StatusResult describeDatabase(const std::string& aName);

	Database* getActiveDatabase() const override;

	static StatusResult databaseExists(bool& exists, const std::string& aName);

private:
	std::unique_ptr<Database> m_activeDB;

	void _resetActiveDB(std::unique_ptr<Database>&& aNewDB);
	void _releaseActiveDB();

	static StatusResult _deleteDBFile(const std::string& aName);
	static StatusResult _collectDBNames(StringList& aDBNames);
};

}

#endif /* DBManager.hpp */