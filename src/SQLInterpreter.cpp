#include "SQLInterpreter.hpp"
#include "Tokenizer.hpp"
#include "SQLStatement.hpp"
#include "Database.hpp"
#include "Row.hpp"

namespace UrSQL {

SQLInterpreter::SQLInterpreter(Interpreter* next) :
	Interpreter(next)
{
}

std::unique_ptr<Statement> SQLInterpreter::getStatement(Tokenizer& aTokenizer) {
	Keyword theKeyword = aTokenizer.peek().getKeyword();

	switch (theKeyword) {
	case Keyword::create_kw: {
		if (aTokenizer.remaining() > 1 && aTokenizer.peek(1).getKeyword() == Keyword::table_kw) {
			return SQLStatement::factory(aTokenizer, *this);
		}
		break;
	}
	case Keyword::describe_kw: {
		if (aTokenizer.remaining() > 1 && aTokenizer.peek(1).getKeyword() == Keyword::table_kw) {
			return SQLStatement::factory(aTokenizer, *this);
		}
		break;
	}
	case Keyword::insert_kw:
		return SQLStatement::factory(aTokenizer, *this);
	case Keyword::select_kw:
		return SQLStatement::factory(aTokenizer, *this);
	}
	return nullptr;
}

StatusResult SQLInterpreter::createTable(const AttributeList& anAttributeList, const std::string& anEntityName) const {
	StatusResult theResult(Error::no_error);
	if (Database* theActiveDB = getActiveDatabase()) {
		theResult = theActiveDB->createTable(anAttributeList, anEntityName);
		if (theResult) {
			theResult.setMessage("Query ok, table '" + anEntityName + "' created");
		}
	}
	else {
		theResult.setError(Error::noDatabase_specified, "Specify the database first by 'use <DBName>'");
	}
	return theResult;
}

StatusResult SQLInterpreter::describeTable(const std::string& anEntityName) const {
	StatusResult theResult(Error::no_error);
	if (Database* theActiveDB = getActiveDatabase()) {
		size_type theAttributeCount;
		theResult = theActiveDB->describeTable(anEntityName, theAttributeCount);
		if (theResult) {
			theResult.setMessage(std::to_string(theAttributeCount) + " row(s) in set");
		}
	}
	else {
		theResult.setError(Error::noDatabase_specified, "Specify the database first by 'use <DBName>'");
	}
	return theResult;
}

StatusResult SQLInterpreter::insertIntoTable(const std::string& anEntityName, const StringList& aFieldNames, const StringList& aValueStrs) const {
	StatusResult theResult(Error::no_error);
	if (Database* theActiveDB = getActiveDatabase()) {
		theResult = theActiveDB->insertIntoTable(anEntityName, aFieldNames, aValueStrs);
		if (theResult) {
			theResult.setMessage("Query ok, " + std::to_string(aFieldNames.size()) + " row(s) inserted");
		}
	}
	else {
		theResult.setError(Error::noDatabase_specified, "Specify the database first by 'use <DBName>'");
	}
	return theResult;
}

StatusResult SQLInterpreter::selectFromTable(const std::string& anEntityName, const StringList& aFieldNames) const {
	StatusResult theResult(Error::no_error);
	if (Database* theActiveDB = getActiveDatabase()) {
		RowCollection theRowCollection;
		theResult = theActiveDB->selectFromTable(theRowCollection, anEntityName, aFieldNames);
		if (theResult) {

		}
	}
	else {
		theResult.setError(Error::noDatabase_specified, "Specify the database first by 'use <DBName>'");
	}
	return theResult;
}

}