#include "DBStatement.hpp"
#include "DBManager.hpp"
#include "Tokenizer.hpp"

namespace UrSQL {

/* -------------------------------CreateDBStatement------------------------------- */
CreateDBStatement::CreateDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager) :
	DBStatement(aTokenizer, aDBManager)
{
}

StatusResult CreateDBStatement::parse() {
	StatusResult theResult(Error::no_error);
	if (m_tokenizer.next(2)) {
		const Token& theToken = m_tokenizer.get();
		if (theToken.get_type() == TokenType::identifier) {
			m_dbname = theToken.get_data();
		}
		else {
			theResult.set_error(Error::unexpected_identifier, "Illegal database name '" + theToken.get_data() + '\'');
		}
	}
	else {
		theResult.set_error(Error::identifier_expected, "Database name expected");
	}
	return theResult;
}

StatusResult CreateDBStatement::validate() const {
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

StatusResult CreateDBStatement::run() const {
	return m_manager.create_database(m_dbname);
}

/* -------------------------------DropDBStatement------------------------------- */
DropDBStatement::DropDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager) :
	DBStatement(aTokenizer, aDBManager)
{
}

StatusResult DropDBStatement::parse() {
	StatusResult theResult(Error::no_error);
	if (m_tokenizer.next(2)) {
		const Token& theToken = m_tokenizer.get();
		if (theToken.get_type() == TokenType::identifier) {
			m_dbname = theToken.get_data();
		}
		else {
			theResult.set_error(Error::unexpected_identifier, "Illegal database name '" + theToken.get_data() + '\'');
		}
	}
	else {
		theResult.set_error(Error::identifier_expected, "Database name expected");
	}
	return theResult;
}

StatusResult DropDBStatement::validate() const {
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

StatusResult DropDBStatement::run() const {
	return m_manager.drop_database(m_dbname);
}

/* -------------------------------UseDBStatement------------------------------- */
UseDBStatement::UseDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager) :
	DBStatement(aTokenizer, aDBManager)
{
}

StatusResult UseDBStatement::parse() {
	StatusResult theResult(Error::no_error);
	if (m_tokenizer.next()) {
		m_tokenizer.skip_if(Keyword::database_kw);
		if (m_tokenizer.more()) {
			const Token& theToken = m_tokenizer.peek();
			if (theToken.get_type() == TokenType::identifier) {
				m_dbname = theToken.get_data();
			}
			else {
				theResult.set_error(Error::unexpected_identifier, "Illegal database name '" + theToken.get_data() + '\'');
			}
		}
		else {
			theResult.set_error(Error::identifier_expected, "Database name expected");
		}
	}
	else {
		theResult.set_error(Error::identifier_expected, "Database name expected");
	}
	return theResult;
}

StatusResult UseDBStatement::validate() const {
	bool exists = false;
	StatusResult theResult = DBManager::database_exists(exists, m_dbname);
	if (theResult && !exists) {
		theResult.set_error(Error::unknown_database, '\'' + m_dbname + '\'');
	}
	return theResult;
}

StatusResult UseDBStatement::run() const {
	return m_manager.use_database(m_dbname);
}

}