#include "controller/BasicInterpreter.hpp"

#include "model/Statement.hpp"
#include "model/Tokenizer.hpp"

namespace UrSQL {

BasicInterpreter::BasicInterpreter(Interpreter* anInterpreter)
    : Interpreter(anInterpreter) {}

std::unique_ptr<Statement> BasicInterpreter::getStatement(
  Tokenizer& aTokenizer) {
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

}  // namespace UrSQL