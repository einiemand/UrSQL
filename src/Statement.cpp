#include "Statement.hpp"
#include "Tokenizer.hpp"

namespace UrSQL {

Statement::Statement(Tokenizer& aTokenizer) :
	m_tokenizer(aTokenizer)
{
}

BasicStatement::BasicStatement(Tokenizer& aTokenizer, Keyword aKeyword) :
	Statement(aTokenizer),
	m_keyword(aKeyword)
{
}

StatusResult BasicStatement::parse() {
	return StatusResult(Error::no_error);
}

StatusResult BasicStatement::validate() const {
	return m_tokenizer.remaining() == 1 ? StatusResult(Error::no_error) :
		StatusResult(Error::invalid_command, "Redundant input after '" + m_tokenizer.peek().get_data() + '\'');
}

StatusResult BasicStatement::run() const {
	StatusResult theResult(Error::no_error);
	switch (m_keyword) {
	case Keyword::help_kw: {
		static const std::string theHelpMsg = 
			"help    - show guide on how to use UrSQL\n"
			"version - show UrSQL version\n"
			"quit    - quit UrSQL";
		theResult.set_message(theHelpMsg);
		break;
	}
	case Keyword::version_kw: {
		static const std::string theVersionMsg = "UrSQL 1.0";
		theResult.set_message(theVersionMsg);
		break;
	}
	case Keyword::quit_kw: {
		theResult.set_error(Error::user_terminated, "Bye");
		break;
	}
	default:
		throw std::runtime_error("Impossible: A BasicStatement shouldn't have been run");
	}
	return theResult;
}

}