#include "parser/SQLBlob.hpp"

#include <format>
#include <iostream>
#include <regex>

#include "common/Finally.hpp"
#include "exception/InternalError.hpp"
#include "exception/UserError.hpp"

namespace ursql {

namespace {

constexpr const char doubleQuote = '"';
constexpr const char singleQuote = '\'';

bool isWhiteSpace(char c) {
    return std::isspace(c);
}

bool isAlpha(char c) {
    return isalpha(c) != 0;
}

bool isDigit(char c) {
    return isdigit(c) != 0;
}

bool isQuote(char c) {
    return c == doubleQuote || c == singleQuote;
}

bool isSeparator(char c) {
    return isWhiteSpace(c) || charIsComparator(c) || isQuote(c) ||
           isPunctuation(c);
}

bool isNumber(const std::string& str) {
    static std::regex numRegex("\\d+(.\\d+)?");
    return std::regex_match(str, numRegex);
}

}  // namespace

bool SQLBlob::ready() const {
    return ready_;
}

TokenStream SQLBlob::tokenize() {
    Finally cleanup([this]() {
        _reset();
    });
    std::vector<Token> tokens;
    for (auto peek = buf_.peek(); std::char_traits<char>::not_eof(peek);
         peek = buf_.peek())
    {
        char ch = std::char_traits<char>::to_char_type(peek);
        if (isWhiteSpace(ch)) {
            buf_.ignore();
        } else if (isAlpha(ch)) {
            std::string str = _readUntil(isSeparator);
            if (isKeyword(str)) {
                tokens.emplace_back(token_type_index<TokenType::keyword>,
                                    toKeyword(str));
            } else {
                tokens.emplace_back(token_type_index<TokenType::identifier>,
                                    std::move(str));
            }
        } else if (isPunctuation(ch)) {
            tokens.emplace_back(token_type_index<TokenType::punctuation>,
                                toPunctuation(ch));
            buf_.ignore();
        } else if (charIsComparator(ch)) {
            std::string str = _readWhile(charIsComparator);
            if (!strIsComparator(str)) {
            }
            URSQL_EXPECT(strIsComparator(str), SyntaxError,
                         std::format("{} is not a comparator", str));
            tokens.emplace_back(token_type_index<TokenType::comparator>,
                                toComparator(str));
        } else if (isDigit(ch)) {
            std::string str = _readUntil(isSeparator);
            URSQL_EXPECT(isNumber(str), SyntaxError,
                         std::format("{} is not a number", str));
            tokens.emplace_back(token_type_index<TokenType::number>,
                                std::stof(str));
        } else if (isOperator(ch)) {
            tokens.emplace_back(token_type_index<TokenType::op>,
                                toOperator(ch));
            buf_.ignore();
        } else if (isQuote(ch)) {
            buf_.ignore();
            std::string str = _readUntil(ch);
            URSQL_EXPECT(buf_.peek() == peek, SyntaxError,
                         std::format("a closing quote {} is missing", ch));
            buf_.ignore();
            if (ch == singleQuote) {
                tokens.emplace_back(token_type_index<TokenType::text>, str);
            } else {
                tokens.emplace_back(token_type_index<TokenType::identifier>,
                                    str);
            }
        } else {
            URSQL_THROW_NORMAL(SyntaxError,
                               std::format("unknown character {}", ch));
        }
    }
    return TokenStream(std::move(tokens));
}

std::istream& operator>>(std::istream& input, SQLBlob& blob) {
    URSQL_ASSERT(!blob.ready(), "A SQLBlob is meant for only one statement");
    for (auto gi = input.get(); std::char_traits<char>::not_eof(gi);
         gi = input.get())
    {
        char ch = std::char_traits<char>::to_char_type(gi);
        if (ch == SQLBlob::delim && !blob.inDoubleQuote_ &&
            !blob.inSingleQuote_)
        {
            blob.ready_ = true;
            break;
        }
        if (ch == doubleQuote) {
            blob.inDoubleQuote_ ^= true;
        } else if (ch == singleQuote) {
            blob.inSingleQuote_ ^= true;
        }
        blob.buf_ << ch;
    }
    return input;
}

std::string SQLBlob::_readUntil(const CharPredicate& pred) {
    std::string str;
    for (auto gi = buf_.get(); std::char_traits<char>::not_eof(gi);
         gi = buf_.get())
    {
        char ch = std::char_traits<char>::to_char_type(gi);
        if (pred(ch)) {
            buf_.unget();
            break;
        }
        str += ch;
    }
    return str;
}

std::string SQLBlob::_readUntil(char c) {
    return _readUntil([c](char ch) {
        return c == ch;
    });
}

std::string SQLBlob::_readWhile(const CharPredicate& pred) {
    return _readUntil([&pred](char c) {
        return !pred(c);
    });
}

void SQLBlob::_reset() {
    *this = SQLBlob();
}

}  // namespace ursql
