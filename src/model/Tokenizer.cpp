#include "model/Tokenizer.hpp"
#include "model/Keyword.hpp"
#include "model/Token.hpp"
#include <algorithm>
#include <cstring>

namespace UrSQL {

constexpr char leftParen = '(';
constexpr char rightParen = ')';
constexpr char comma = ',';
constexpr char doubleQuote = '"';
constexpr char singleQuote = '\'';
constexpr char star = '*';

Tokenizer::Tokenizer(std::istream& anInput) :
	m_input(anInput),
	m_tokens(),
	m_index(0)
{
}

inline bool isWhiteSpace(char aChar) {
	return std::isspace(aChar);
}

inline bool isAlpha(char aChar) {
	return isalpha(aChar) != 0;
}

inline bool isNumber(char aChar) {
	return isdigit(aChar) || aChar == '.';
}

inline bool isComparator(char aChar) {
	return strchr("!=<>", aChar) != nullptr;
}

inline bool isQuote(char aChar) {
	return aChar == doubleQuote || aChar == singleQuote;
}

inline bool isPunctuation(char aChar) {
	return strchr(",()*", aChar) != nullptr;
}

TokenType getPunctuationType(char aChar) {
	switch (aChar) {
	case comma:
		return TokenType::comma;
	case leftParen:
		return TokenType::lparen;
	case rightParen:
		return TokenType::rparen;
	case star:
		return TokenType::star;
	default:
		return TokenType::unknown;
	}
}

inline bool isSeparator(char aChar) {
	return isWhiteSpace(aChar) || isComparator(aChar) || isQuote(aChar) || isPunctuation(aChar);
}

const Token& Tokenizer::peek(size_type anOffset) {
	URSQL_TRUTH(remaining() > anOffset, "Check if there are more tokens before peeking!");
	return m_tokens[m_index + anOffset];
}

const Token& Tokenizer::get() {
	const Token& theToken = peek();
	++m_index;
	return theToken;
}

bool Tokenizer::next(size_type anOffset) {
	m_index += anOffset;
	return more();
}

bool Tokenizer::skipIf(SkipCondition aCondition) {
	if (more() && aCondition(peek())) {
		next();
		return true;
	}
	return false;
}

StatusResult Tokenizer::tokenize() {
	StatusResult theResult(Error::no_error);
	for (char peek = m_input.peek(); theResult && peek != -1; peek = m_input.peek()) {
		if (isWhiteSpace(peek)) {
			m_input.get();
		}
		else if (isAlpha(peek)) {
			std::string theData = _readUntil(isSeparator);
			std::string theLowerCaseData(theData);
			std::transform(theLowerCaseData.begin(), theLowerCaseData.end(), theLowerCaseData.begin(), ::tolower);
			if (isKeyword(theLowerCaseData)) {
				m_tokens.emplace_back(TokenType::keyword, theLowerCaseData, toKeyword(theLowerCaseData));
			}
			else {
				m_tokens.emplace_back(TokenType::identifier, std::move(theData));
			}
		}
		else if (isPunctuation(peek)) {
			TokenType theType = getPunctuationType(peek);
			m_tokens.emplace_back(theType, std::string(1, m_input.get()));
		}
		else if (isNumber(peek)) {
			std::string theData = _readUntil(isSeparator);
			TokenType theType = TokenType::number;
			if (!std::all_of(theData.cbegin(), theData.cend(), isNumber)) {
				theType = TokenType::identifier;
			}
			m_tokens.emplace_back(theType, std::move(theData));
		}
		else if (isComparator(peek)) {
			std::string theData = _readWhile(isComparator);
			m_tokens.emplace_back(TokenType::comparator, std::move(theData));
		}
		else if (isQuote(peek)) {
			m_input.get();
			std::string theData = _readUntil(peek);
			if (m_input.peek() == peek) {
				m_input.get();
				m_tokens.emplace_back(TokenType::string, std::move(theData));
			}
			else {
				theResult.setError(Error::syntax_error, "A terminating quote is expected");
			}
		}
		else {
			std::string theData = _readUntil(-1);
			theResult.setError(Error::syntax_error, "Unknown command - '" + theData + '\'');
		}
	}
	return theResult;
}

std::string Tokenizer::_readWhile(TokenizeCondition aCondition) {
	std::string theData;
	for (char theChar = m_input.peek(); theChar != -1 && aCondition(theChar); theChar = m_input.peek()) {
		theData += static_cast<char>(m_input.get());
	}
	return theData;
}

std::string Tokenizer::_readUntil(TokenizeCondition aCondition) {
	std::string theData;
	for (char theChar = m_input.peek(); theChar != -1 && !aCondition(theChar); theChar = m_input.peek()) {
		theData += static_cast<char>(m_input.get());
	}
	return theData;
}

} /* UrSQL */