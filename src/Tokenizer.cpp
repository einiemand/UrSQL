#include "Tokenizer.hpp"
#include "Keyword.hpp"

namespace UrSQL {

constexpr char left_paren = '(';
constexpr char right_paren = ')';
constexpr char comma = ',';
constexpr char double_quote = '"';
constexpr char single_quote = '\'';

Tokenizer::Tokenizer(std::istream& anInput) :
	m_input(anInput),
	m_tokens(),
	m_index(0)
{
}

inline bool is_whitespace(char aChar) {
	return strchr(" \t\r\n\b", aChar) != nullptr;
}

inline bool is_number(char aChar) {
	return isdigit(aChar) || aChar == '.';
}

inline bool is_comparator(char aChar) {
	return strchr("=<>", aChar) != nullptr;
}

inline bool is_quote(char aChar) {
	return aChar == double_quote || aChar == single_quote;
}

inline bool is_punctuation(char aChar) {
	return strchr(",()", aChar) != nullptr;
}

inline bool is_separator(char aChar) {
	return is_whitespace(aChar) || is_comparator(aChar) || is_quote(aChar) || is_punctuation(aChar);
}

void Tokenizer::tokenize() {
	for (char peek = m_input.peek(); !m_input.eof() && peek != -1; peek = m_input.peek()) {

	}
}

std::string Tokenizer::read_while(tokenize_condition aCondition) {
	char theChar;
	std::string theData;
	while (m_input.get(theChar) && aCondition(theChar)) {
		theData += theChar;
	}
	return theData;
}

std::string Tokenizer::read_until(tokenize_condition aCondition) {
	char theChar;
	std::string theData;
	while (m_input.get(theChar) && !aCondition(theChar)) {
		theData += theChar;
	}
	return theData;
}

} /* UrSQL */