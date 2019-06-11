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
	return std::isspace(aChar);
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

Token& Tokenizer::peek() {
	if (more()) {
		return m_tokens.front();
	}
	throw std::out_of_range("Check if there are more tokens before peeking!");
}

Token& Tokenizer::get() {
	Token& theToken = peek();
	++m_index;
	return theToken;
}

StatusResult Tokenizer::tokenize() {
	StatusResult theResult(Error::no_error);
	for (char peek = m_input.peek(); theResult && peek != -1; peek = m_input.peek()) {
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
				m_tokens.emplace_back(TokenType::identifier, std::move(theData));
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
			m_input.get();
			std::string theData = read_until(peek);
			if (m_input.peek() == peek) {
				m_input.get();
				m_tokens.emplace_back(TokenType::identifier, std::move(theData));
			}
			else {
				theResult.set_error(Error::syntax_error, "A terminating quote is expected");
			}
		}
		else {
			std::string theData = read_until(-1);
			theResult.set_error(Error::syntax_error, "Unknown command - '" + theData + '\'');
		}
	}
	return theResult;
}

std::string Tokenizer::read_while(TokenizeCondition aCondition) {
	std::string theData;
	for (char theChar = m_input.peek(); theChar != -1 && aCondition(theChar); theChar = m_input.peek()) {
		theData += m_input.get();
	}
	return theData;
}

std::string Tokenizer::read_until(TokenizeCondition aCondition) {
	std::string theData;
	for (char theChar = m_input.peek(); theChar != -1 && !aCondition(theChar); theChar = m_input.peek()) {
		theData += m_input.get();
	}
	return theData;
}

} /* UrSQL */