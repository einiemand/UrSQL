#include "DBStatement.hpp"
#include "DBManager.hpp"
#include "Tokenizer.hpp"

namespace UrSQL {


/* -------------------------------CreateDBStatement------------------------------- */
class CreateDBStatement : public DBStatement {
public:
	CreateDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager) :
		DBStatement(aTokenizer, aDBManager)
	{
	}

	~CreateDBStatement() override = default;

	StatusResult parse() override {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.next(2)) {
			const Token& theToken = m_tokenizer.get();
			switch (theToken.get_type()) {
			case TokenType::identifier:
				m_dbname = theToken.get_data();
				break;
			case TokenType::string:
				theResult.set_error(Error::syntax_error, "Don't surround identifiers with quotes");
				break;
			default:
				theResult.set_error(Error::unexpected_identifier, "Illegal database name '" + theToken.get_data() + '\'');
			}
		}
		else {
			theResult.set_error(Error::identifier_expected, "Database name expected");
		}
		return theResult;
	}

	StatusResult validate() const override {
		StatusResult theResult(Error::no_error);
		if (!m_tokenizer.more()) {
			bool exists = false;
			theResult = DBManager::database_exists(exists, m_dbname);
			if (theResult && exists) {
				theResult.set_error(Error::database_exists, '\'' + m_dbname + '\'');
			}
		}
		else {
			theResult.set_error(Error::invalid_command, "Redundant input after '" + m_dbname + '\'');
		}
		return theResult;
	}

	StatusResult execute() const override {
		return m_manager.create_database(m_dbname);
	}
private:
	std::string m_dbname;
};


/* -------------------------------DropDBStatement------------------------------- */
class DropDBStatement : public DBStatement {
public:
	DropDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager) :
		DBStatement(aTokenizer, aDBManager)
	{
	}

	~DropDBStatement() override = default;

	StatusResult parse() override {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.next(2)) {
			const Token& theToken = m_tokenizer.get();
			switch (theToken.get_type()) {
			case TokenType::identifier:
				m_dbname = theToken.get_data();
				break;
			case TokenType::string:
				theResult.set_error(Error::syntax_error, "Don't surround identifiers with quotes");
				break;
			default:
				theResult.set_error(Error::unexpected_identifier, "Illegal database name '" + theToken.get_data() + '\'');
			}
		}
		else {
			theResult.set_error(Error::identifier_expected, "Database name expected");
		}
		return theResult;
	}

	StatusResult validate() const override {
		StatusResult theResult(Error::no_error);
		if (!m_tokenizer.more()) {
			bool exists = false;
			theResult = DBManager::database_exists(exists, m_dbname);
			if (theResult && !exists) {
				theResult.set_error(Error::unknown_database, '\'' + m_dbname + '\'');
			}
		}
		else {
			theResult.set_error(Error::invalid_command, "Redundant input after '" + m_dbname + '\'');
		}
		return theResult;
	}

	StatusResult execute() const override {
		return m_manager.drop_database(m_dbname);
	}
private:
	std::string m_dbname;
};


/* -------------------------------UseDBStatement------------------------------- */
class UseDBStatement : public DBStatement {
public:
	UseDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager) :
		DBStatement(aTokenizer, aDBManager)
	{
	}

	~UseDBStatement() override = default;

	StatusResult parse() override {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.next()) {
			const Token& theToken = m_tokenizer.get();
			switch (theToken.get_type()) {
			case TokenType::identifier:
				m_dbname = theToken.get_data();
				break;
			case TokenType::string:
				theResult.set_error(Error::syntax_error, "Don't surround identifiers with quotes");
				break;
			default:
				theResult.set_error(Error::unexpected_identifier, "Illegal database name '" + theToken.get_data() + '\'');
			}
		}
		else {
			theResult.set_error(Error::identifier_expected, "Database name expected");
		}
		return theResult;
	}

	StatusResult validate() const override {
		StatusResult theResult(Error::no_error);
		if (!m_tokenizer.more()) {
			bool exists = false;
			theResult = DBManager::database_exists(exists, m_dbname);
			if (theResult && !exists) {
				theResult.set_error(Error::unknown_database, '\'' + m_dbname + '\'');
			}
		}
		else {
			theResult.set_error(Error::invalid_command, "Redundant input after '" + m_dbname + '\'');
		}
		return theResult;
	}

	StatusResult execute() const override {
		return m_manager.use_database(m_dbname);
	}
private:
	std::string m_dbname;
};


/* -------------------------------ShowDBStatement------------------------------- */
class ShowDBStatement : public DBStatement {
public:
	ShowDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager) :
		DBStatement(aTokenizer, aDBManager)
	{
	}

	~ShowDBStatement() override = default;

	StatusResult parse() override {
		m_tokenizer.next(2);
		return StatusResult(Error::no_error);
	}

	StatusResult validate() const override {
		return !m_tokenizer.more() ? StatusResult(Error::no_error) :
			StatusResult(Error::invalid_command, "Redundant input after 'databases'");
	}

	StatusResult execute() const override {
		return m_manager.show_databases();
	}
};


/* -------------------------------DescDBStatement------------------------------- */
class DescDBStatement : public DBStatement {
public:
	DescDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager) :
		DBStatement(aTokenizer, aDBManager)
	{
	}

	~DescDBStatement() override = default;

	StatusResult parse() override {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.next(2)) {
			const Token& theToken = m_tokenizer.get();
			switch (theToken.get_type()) {
			case TokenType::identifier:
				m_dbname = theToken.get_data();
				break;
			case TokenType::string:
				theResult.set_error(Error::syntax_error, "Don't surround identifiers with quotes");
				break;
			default:
				theResult.set_error(Error::unexpected_identifier, "Illegal database name '" + theToken.get_data() + '\'');
			}
		}
		else {
			theResult.set_error(Error::identifier_expected, "Database name expected");
		}
		return theResult;
	}

	StatusResult validate() const override {
		StatusResult theResult(Error::no_error);
		if (!m_tokenizer.more()) {
			bool exists = false;
			theResult = DBManager::database_exists(exists, m_dbname);
			if (theResult && !exists) {
				theResult.set_error(Error::unknown_database, '\'' + m_dbname + '\'');
			}
		}
		else {
			theResult.set_error(Error::invalid_command, "Redundant input after '" + m_dbname + '\'');
		}
		return theResult;
	}

	StatusResult execute() const override {
		return m_manager.describe_database(m_dbname);
	}
private:
	std::string m_dbname;
};


/* -------------------------------Static DBStatement Methods------------------------------- */
std::unique_ptr<DBStatement> DBStatement::factory(Keyword aStmtType, Tokenizer& aTokenizer, DBManager& aDBManager) {
	switch (aStmtType) {
	case Keyword::create_kw:
		return std::make_unique<CreateDBStatement>(aTokenizer, aDBManager);
	case Keyword::drop_kw:
		return std::make_unique<DropDBStatement>(aTokenizer, aDBManager);
	case Keyword::use_kw:
		return std::make_unique<UseDBStatement>(aTokenizer, aDBManager);
	case Keyword::show_kw:
		return std::make_unique<ShowDBStatement>(aTokenizer, aDBManager);
	case Keyword::describe_kw:
		return std::make_unique<DescDBStatement>(aTokenizer, aDBManager);
	default:
		return nullptr;
	}
}

}