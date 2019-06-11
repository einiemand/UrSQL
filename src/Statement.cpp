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
	switch (m_keyword) {
	case Keyword::help_kw: {
		static const char* theHelpMsg = 
			"help    - show guide on how to use UrSQL.\n"
			"version - show UrSQL version.\n"
			"quit    - quit UrSQL.";
		show_message(theHelpMsg);
		break;
	}
	case Keyword::version_kw:
		show_message("UrSQL 1.0");
		break;
	case Keyword::quit_kw:
		show_message("Bye.");
		return StatusResult(Error::user_terminated);
	case Keyword::unknown_kw:
		break;
	default:
		throw std::runtime_error("Impossible: A BasicStatement shouldn't have been run");
	}
	return StatusResult(Error::no_error);
}

}