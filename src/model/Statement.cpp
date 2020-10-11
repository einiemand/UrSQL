#include "model/Statement.hpp"

#include "model/Tokenizer.hpp"

namespace UrSQL {

Statement::Statement(Tokenizer& aTokenizer) : m_tokenizer(aTokenizer) {}

StatusResult Statement::run() {
    StatusResult theResult = parse();
    if (theResult) {
        theResult = validate();
        if (theResult) {
            theResult = execute();
        }
    }
    return theResult;
}

BasicStatement::BasicStatement(Tokenizer& aTokenizer, Keyword aKeyword)
    : Statement(aTokenizer),
      m_keyword(aKeyword) {}

StatusResult BasicStatement::parse() {
    return StatusResult(Error::no_error);
}

StatusResult BasicStatement::validate() const {
    return m_tokenizer.remaining() == 1 ?
             StatusResult(Error::no_error) :
             StatusResult(
               Error::invalid_command,
               "Redundant input after '" + m_tokenizer.peek().getData() + '\'');
}

StatusResult BasicStatement::execute() const {
    StatusResult theResult(Error::no_error);
    switch (m_keyword) {
    case Keyword::help_kw: {
        static const std::string theHelpMsg =
          "help    - show guide on how to use UrSQL\n"
          "version - show UrSQL version\n"
          "quit    - quit UrSQL";
        theResult.setMessage(theHelpMsg);
        break;
    }
    case Keyword::version_kw: {
        static const std::string theVersionMsg = "UrSQL 1.0";
        theResult.setMessage(theVersionMsg);
        break;
    }
    case Keyword::quit_kw: {
        theResult.setError(Error::user_terminated, "Bye");
        break;
    }
    default:
        URSQL_TRUTH(false, "A BasicStatement shouldn't have been run");
    }
    return theResult;
}

}  // namespace UrSQL
