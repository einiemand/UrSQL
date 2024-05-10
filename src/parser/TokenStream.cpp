#include <format>
#include <iostream>
#include <regex>

#include "exception/UserError.hpp"
#include "parser/TokenStream.hpp"

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

using CharPredicate = std::function<bool(char)>;

std::string readUntil(std::istream& input, const CharPredicate& pred) {
    std::string str;
    for (auto peek = input.peek(); std::char_traits<char>::not_eof(peek);
         peek = input.peek())
    {
        char c = std::char_traits<char>::to_char_type(peek);
        if (pred(c)) {
            break;
        }
        str += c;
        input.ignore();
    }
    return str;
}

std::string readUntil(std::istream& input, char c) {
    return readUntil(input, [c](char ch) {
        return c == ch;
    });
}

std::string readWhile(std::istream& input, const CharPredicate& pred) {
    return readUntil(input, [&](char c) {
        return !pred(c);
    });
}

}  // namespace

std::vector<Token> TokenStream::tokenize(std::istream& input) {
    std::vector<Token> tokens;
    for (auto peek = input.peek(); std::char_traits<char>::not_eof(peek);
         peek = input.peek())
    {
        char ch = std::char_traits<char>::to_char_type(peek);
        if (isWhiteSpace(ch)) {
            input.ignore();
        } else if (isAlpha(ch)) {
            std::string str = readUntil(input, isSeparator);
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
            input.ignore();
        } else if (charIsComparator(ch)) {
            std::string str = readWhile(input, charIsComparator);
            URSQL_EXPECT(strIsComparator(str), SyntaxError,
                         std::format("{} is not a comparator", str));
            tokens.emplace_back(token_type_index<TokenType::comparator>,
                                toComparator(str));
        } else if (isDigit(ch)) {
            std::string str = readUntil(input, isSeparator);
            URSQL_EXPECT(isNumber(str), SyntaxError,
                         std::format("{} is not a number", str));
            tokens.emplace_back(token_type_index<TokenType::number>,
                                std::stof(str));
        } else if (isOperator(ch)) {
            tokens.emplace_back(token_type_index<TokenType::op>,
                                toOperator(ch));
            input.ignore();
        } else if (isQuote(ch)) {
            input.ignore();
            std::string str = readUntil(input, ch);
            URSQL_EXPECT(input.peek() == peek, SyntaxError,
                         std::format("a closing quote {} is missing", ch));
            input.ignore();
            if (ch == singleQuote) {
                tokens.emplace_back(token_type_index<TokenType::text>, str);
            } else {
                tokens.emplace_back(token_type_index<TokenType::identifier>,
                                    str);
            }
        } else {
            URSQL_FAIL(SyntaxError, std::format("unknown character {}", ch));
        }
    }
    return tokens;
}

TokenStream::TokenStream(std::istream& input)
    : tokens_(tokenize(input)), i_(0) {}

bool TokenStream::hasNext() const noexcept {
    return remaining() > 0;
}

const Token& TokenStream::next() {
    return tokens_[i_++];
}

std::size_t TokenStream::remaining() const noexcept {
    return i_ < tokens_.size() ? tokens_.size() - i_ : 0;
}

const Token& TokenStream::peek() const {
    return tokens_[i_];
}

bool TokenStream::skipIf(const TokenPredicate& pred) {
    if (hasNext() && pred(peek())) {
        ++i_;
        return true;
    }
    return false;
}

bool TokenStream::skipIf(Keyword keyword) {
    return skipIf([keyword](const Token& token) {
        return token.is<TokenType::keyword>(keyword);
    });
}

bool TokenStream::skipIf(Punctuation punctuation) {
    return skipIf([punctuation](const Token& token) {
        return token.is<TokenType::punctuation>(punctuation);
    });
}

}  // namespace ursql
