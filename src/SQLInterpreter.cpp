#include "SQLInterpreter.hpp"
#include "Tokenizer.hpp"
#include "SQLStatement.hpp"
#include "Database.hpp"

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
	}
	return nullptr;
}

StatusResult SQLInterpreter::createTable(const AttributeList& anAttributeList, const std::string& anEntityName) {
	StatusResult theResult(Error::no_error);
	if (Database* theActiveDB = getActiveDatabase()) {
		theResult = theActiveDB->createTable(anAttributeList, anEntityName);
	}
	else {
		theResult.setError(Error::unknown_database, "Specify the database first by 'use <DBName>'");
	}
	return theResult;
}

}