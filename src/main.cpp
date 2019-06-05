#include "Error.hpp"
#include "Tokenizer.hpp"
#include <sstream>
using namespace UrSQL;

int main(int argc, char* argv[])
{
	std::string userInput;
	while (true) {
		defaultOutput << "> ";
		while (std::getline(std::cin, userInput, ';')) {
			std::istringstream inputStream(userInput);
			Tokenizer theTokenizer(inputStream);
			theTokenizer.tokenize();
			Token theToken = theTokenizer.current();
			std::cout << theToken.get_data() << '\n';
		}
	}

	return 0;
}