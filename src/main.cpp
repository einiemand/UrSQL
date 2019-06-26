#include "Error.hpp"
#include "Tokenizer.hpp"
#include "BasicProcessor.hpp"
#include "DBManager.hpp"
#include "SQLInterpreter.hpp"
#include "Database.hpp"
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

namespace UrSQL {
class TimeCounter {
public:
	using Clock = std::chrono::steady_clock;

	TimeCounter() : m_start(Clock::now()) {}
	~TimeCounter() { _show_time_elapsed(); }

	TimeCounter(const TimeCounter&) = delete;
	TimeCounter& operator=(const TimeCounter&) = delete;
private:
	std::chrono::time_point<Clock> m_start;

	void _show_time_elapsed() {
		auto theEndTime = Clock::now();
		auto theElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(theEndTime - m_start);

		defaultOutput << " (" << std::setprecision(2) << std::fixed <<
			static_cast<float>(theElapsedTime.count()) / 1000 << " sec)\n";
	}
};

void trim_input(std::string& aString) {
	aString.erase(aString.begin(), std::find_if_not(aString.begin(), aString.end(), std::isspace));
	aString.erase(aString.length() - (std::find_if_not(aString.rbegin(), aString.rend(), std::isspace) - aString.rbegin()));
}

}

int main(int argc, char* argv[])
{
	using namespace UrSQL;
	static const char* const thePrevMark = "\nursql> ";
	DBManager theDBManager;
	SQLInterpreter theSQLInterpreter(&theDBManager);
	BasicProcessor theBasicProcessor(&theSQLInterpreter);

	StatusResult theResult(Error::no_error);
	while (theResult.getCode() != Error::user_terminated) {
		defaultOutput << thePrevMark;
		std::string theUserInput;
		if (std::getline(std::cin, theUserInput, ';')) {
			TimeCounter theCounter;
			trim_input(theUserInput);
			if (!theUserInput.empty()) {
				std::istringstream theInputStream(theUserInput);
				Tokenizer theTokenizer(theInputStream);
				theResult = theTokenizer.tokenize();
				if (theResult) {
					theResult = theBasicProcessor.process_input(theTokenizer);
				}
				theResult.showError();
			}
		}

	}
	/*char theChar;

	while (theResult.getCode() != Error::user_terminated) {
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
			if (!theResult && theResult.getCode() != Error::user_terminated) {
				theResult.showError();
			}
		}
	}*/

	return 0;
}