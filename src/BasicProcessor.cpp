#include "BasicProcessor.hpp"
#include "Statement.hpp"
#include "Tokenizer.hpp"

namespace UrSQL {

BasicProcessor::BasicProcessor(Interpreter* anInterpreter) :
	Interpreter(anInterpreter)
{
}

std::unique_ptr<Statement> BasicProcessor::getStatement(Tokenizer& aTokenizer) {
	Keyword theKeyword = aTokenizer.peek().get_keyword();
	switch (theKeyword) {
	case Keyword::help_kw:
	case Keyword::version_kw:
	case Keyword::quit_kw:
		return std::make_unique<BasicStatement>(aTokenizer, theKeyword);
	default:
		return nullptr;
	}
}

}