#include "Tokenizer.hpp"
#include "Keyword.hpp"
#include "Token.hpp"
#include <algorithm>

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

inline bool is_alpha(char aChar) {
	return isalpha(aChar) != 0;
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

TokenType get_punctuation_type(char aChar) {
	switch (aChar) {
	case comma:
		return TokenType::comma;
	case left_paren:
		return TokenType::lparen;
	case right_paren:
		return TokenType::rparen;
	default:
		return TokenType::unknown;
	}
}

inline bool is_separator(char aChar) {
	return is_whitespace(aChar) || is_comparator(aChar) || is_quote(aChar) || is_punctuation(aChar);
}

void Tokenizer::tokenize() {
	for (char peek = m_input.peek(); !m_input.eof() && peek != -1; peek = m_input.peek()) {
		if (is_whitespace(peek)) {
			m_input.get();
		}
		else if (is_alpha(peek)) {
			std::string theData = read_until(is_separator);
			std::string theLowerCaseData(theData);
			std::transform(theLowerCaseData.begin(), theLowerCaseData.end(), theLowerCaseData.begin(), ::tolower);
			if (is_keyword(theLowerCaseData)) {
				m_tokens.emplace_back(TokenType::keyword, theLowerCaseData, to_keyword(theLowerCaseData));
			}
			else {
				m_tokens.emplace_back(TokenType::identifier, theData);
			}
		}
		else if (is_punctuation(peek)) {
			TokenType theType = get_punctuation_type(peek);
			m_tokens.emplace_back(theType, std::string(1, m_input.get()));
		}
		else if (is_number(peek)) {
			std::string theData = read_until(is_separator);
			TokenType theType = TokenType::number;
			if (!std::all_of(theData.cbegin(), theData.cend(), is_number)) {
				theType = TokenType::identifier;
			}
			m_tokens.emplace_back(theType, std::move(theData));
		}
		else if (is_comparator(peek)) {
			std::string theData = read_while(is_comparator);
			m_tokens.emplace_back(TokenType::comparator, std::move(theData));
		}
		else if (is_quote(peek)) {
			std::string theData = read_while(is_quote);
			m_tokens.emplace_back(TokenType::identifier, std::move(theData));
		}
		else {
			std::string theData = read_until(is_separator);
			m_tokens.emplace_back(TokenType::unknown, std::move(theData));
		}
	}
}

std::string Tokenizer::read_while(tokenize_condition aCondition) {
	std::string theData;
	for (char theChar = m_input.peek(); theChar != -1 && aCondition(theChar); theChar = m_input.peek()) {
		theData += m_input.get();
	}
	return theData;
}

std::string Tokenizer::read_until(tokenize_condition aCondition) {
	std::string theData;
	for (char theChar = m_input.peek(); theChar != -1 && !aCondition(theChar); theChar = m_input.peek()) {
		theData += m_input.get();
	}
	return theData;
}

} /* UrSQL */