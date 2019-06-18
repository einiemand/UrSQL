#include "SQLStatement.hpp"

namespace UrSQL {

SQLStatement::SQLStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter) :
	Statement(aTokenizer),
	m_interpreter(anInterpreter)
{
}


/* -------------------------------CreateTableStatement------------------------------- */
class CreateTableStatement : public SQLStatement {
public:
	CreateTableStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter) :
		SQLStatement(aTokenizer, anInterpreter)
	{
	}

private:
};

}