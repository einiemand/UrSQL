#include "parser/Tokenizer.hpp"

#include <algorithm>
#include <cstring>

#include "parser/Token.hpp"

namespace ursql {

constexpr char leftParen = '(';
constexpr char rightParen = ')';
constexpr char comma = ',';
constexpr char doubleQuote = '"';
constexpr char singleQuote = '\'';

Tokenizer::Tokenizer(std::istream& anInput)
    : m_input(anInput) {}

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

Punctuation getPunctuationType(char aChar) {
    switch (aChar) {
    case comma:
        return Punctuation::comma;
    case leftParen:
        return Punctuation::lparen;
    case rightParen:
        return Punctuation::rparen;
    default:
        URSQL_UNREACHABLE;
    }
}

inline bool isSeparator(char aChar) {
    return isWhiteSpace(aChar) || isComparator(aChar) || isQuote(aChar) ||
           isPunctuation(aChar);
}
//
//StatusResult Tokenizer::tokenize() {
//    StatusResult theResult(Error::no_error);
//    for (char peek = m_input.peek(); theResult && peek != -1;
//         peek = m_input.peek())
//    {
//        if (isWhiteSpace(peek)) {
//            m_input.get();
//        } else if (isAlpha(peek)) {
//            std::string theData = _readUntil(isSeparator);
//            std::string theLowerCaseData(theData);
//            std::transform(theLowerCaseData.begin(), theLowerCaseData.end(),
//                           theLowerCaseData.begin(), ::tolower);
//            if (isKeyword(theLowerCaseData)) {
//                m_tokens.emplace_back(TokenType::keyword, theLowerCaseData,
//                                      toKeyword(theLowerCaseData));
//            } else {
//                m_tokens.emplace_back(TokenType::identifier,
//                                      std::move(theData));
//            }
//        } else if (isPunctuation(peek)) {
//            TokenType theType = getPunctuationType(peek);
//            m_tokens.emplace_back(theType, std::string(1, m_input.get()));
//        } else if (isNumber(peek)) {
//            std::string theData = _readUntil(isSeparator);
//            TokenType theType = TokenType::number;
//            if (!std::all_of(theData.cbegin(), theData.cend(), isNumber)) {
//                theType = TokenType::identifier;
//            }
//            m_tokens.emplace_back(theType, std::move(theData));
//        } else if (isComparator(peek)) {
//            std::string theData = _readWhile(isComparator);
//            m_tokens.emplace_back(TokenType::comparator, std::move(theData));
//        } else if (isQuote(peek)) {
//            m_input.get();
//            std::string theData = _readUntil(peek);
//            if (m_input.peek() == peek) {
//                m_input.get();
//                m_tokens.emplace_back(TokenType::string, std::move(theData));
//            } else {
//                theResult.setError(Error::syntax_error,
//                                   "A terminating quote is expected");
//            }
//        } else {
//            std::string theData = _readUntil(-1);
//            theResult.setError(Error::syntax_error,
//                               "Unknown command - '" + theData + '\'');
//        }
//    }
//    return theResult;
//}

std::string Tokenizer::_readWhile(TokenizeCondition aCondition) {
    std::string theData;
    for (char theChar = m_input.peek(); theChar != -1 && aCondition(theChar);
         theChar = m_input.peek())
    {
        theData += static_cast<char>(m_input.get());
    }
    return theData;
}

std::string Tokenizer::_readUntil(TokenizeCondition aCondition) {
    std::string theData;
    for (char theChar = m_input.peek(); theChar != -1 && !aCondition(theChar);
         theChar = m_input.peek())
    {
        theData += static_cast<char>(m_input.get());
    }
    return theData;
}

}  // namespace ursql
