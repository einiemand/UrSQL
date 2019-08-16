#include "controller/BasicProcessor.hpp"
#include "model/Statement.hpp"
#include "model/Tokenizer.hpp"

namespace UrSQL {

BasicProcessor::BasicProcessor(Interpreter* anInterpreter) :
	Interpreter(anInterpreter)
{
}

std::unique_ptr<Statement> BasicProcessor::getStatement(Tokenizer& aTokenizer) {
	Keyword theKeyword = aTokenizer.peek().getKeyword();
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