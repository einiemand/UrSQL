#include "Error.hpp"
#include "Tokenizer.hpp"
#include "BasicProcessor.hpp"
#include "DBManager.hpp"
#include "Database.hpp"
#include <sstream>
#include <algorithm>

void trim(std::string& aString) {
	aString.erase(aString.begin(), std::find_if_not(aString.begin(), aString.end(), std::isspace));
	aString.erase(aString.length() - (std::find_if_not(aString.rbegin(), aString.rend(), std::isspace) - aString.rbegin()));
}

int main(int argc, char* argv[])
{
	using namespace UrSQL;
	static const char* const thePrevMark = "\n> ";
	DBManager theDBManager;
	BasicProcessor theBasicProcessor(&theDBManager);

	StatusResult theResult(Error::no_error);
	while (theResult.get_code() != Error::user_terminated) {
		defaultOutput << thePrevMark;
		std::string theUserInput;
		if (std::getline(std::cin, theUserInput, ';')) {
			trim(theUserInput);
			if (!theUserInput.empty()) {
				std::istringstream theInputStream(theUserInput);
				Tokenizer theTokenizer(theInputStream);
				theResult = theTokenizer.tokenize();
				if (theResult) {
					theResult = theBasicProcessor.process_input(theTokenizer);
				}
				if (theResult.get_code() != Error::user_terminated) {
					theResult.show_error();
				}
			}
		}
	}
	/*char theChar;

	while (theResult.get_code() != Error::user_terminated) {
		defaultOutput << thePrevMark;
		std::string theUserInput;
		while (std::cin.get(theChar) && theChar != ';') {
			theUserInput += theChar;
			if (theChar == '\n') {
				defaultOutput << thePrevMark;
			}
		}
		std::cin.ignore();

		trim(theUserInput);
		if (!theUserInput.empty()) {
			std::istringstream theInputStream(theUserInput);
			Tokenizer theTokenizer(theInputStream);
			theResult = theTokenizer.tokenize();
			if (theResult) {
				theResult = theBasicProcessor.process_input(theTokenizer);
			}
			if (!theResult && theResult.get_code() != Error::user_terminated) {
				theResult.show_error();
			}
		}
	}*/

	return 0;
}