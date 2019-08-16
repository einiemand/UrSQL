#include "controller/Processor.hpp"
#include "model/Tokenizer.hpp"
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <sstream>

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

void Processor::consume(std::istream& anInput, CommandConsumer aConsumer) {
	static const char* const thePrevMark = "\nursql> ";
	StatusResult theResult(Error::no_error);
	while (theResult.getCode() != Error::user_terminated && !anInput.eof()) {
		defaultOutput << thePrevMark;
		std::string theUserInput;
		if (std::getline(anInput, theUserInput, ';')) {
			theResult = aConsumer(theUserInput);
		}
	}
}

StatusResult Processor::consumeOne(const std::string& aCommandString) {
	TimeCounter aCounter;
	StatusResult theResult(Error::no_error);
	if (std::any_of(aCommandString.cbegin(), aCommandString.cend(),
		[](char aChar) { return !std::isspace(aChar); })
		)
	{
		std::istringstream theInputStream(aCommandString);
		Tokenizer theTokenizer(theInputStream);
		theResult = theTokenizer.tokenize();
		if (theResult) {
			theResult = m_interpreters.front()->processInput(theTokenizer);
		}
	}
	theResult.showError();
	return theResult;
}

}