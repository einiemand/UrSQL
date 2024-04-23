#include "parser/TokenIterator.hpp"
#include <istream>
#include <regex>

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
    for (auto peek = input.peek(); std::char_traits<char>::not_eof(peek); peek = input.peek()) {
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
    return readUntil(input, [c](char ch) { return c == ch; });
}

std::string readWhile(std::istream& input, const CharPredicate& pred) {
    return readUntil(input, [&](char c) { return !pred(c); });
}

}

TokenIterator TokenIterator::tokenize(std::istream& input) {
    std::vector<Token> tokens;
    for (auto peek = input.peek(); std::char_traits<char>::not_eof(peek); peek = input.peek()) {
        char ch = std::char_traits<char>::to_char_type(peek);
        if (isWhiteSpace(ch)) {
            input.ignore();
        } else if (isAlpha(ch)) {
            std::string str = readUntil(input, isSeparator);
            std::transform(std::begin(str), std::end(str), std::begin(str), ::tolower);
            if (isKeyword(str)) {
                tokens.emplace_back(token_type_index<TokenType::keyword>, toKeyword(str));
            } else {
                tokens.emplace_back(token_type_index<TokenType::identifier>, std::move(str));
            }
        } else if (isPunctuation(ch)) {
            tokens.emplace_back(token_type_index<TokenType::punctuation>, toPunctuation(ch));
            input.ignore();
        } else if (charIsComparator(ch)) {
            std::string str = readWhile(input, charIsComparator);
            if (!strIsComparator(str)) {
                // TODO: throw here
            }
            tokens.emplace_back(token_type_index<TokenType::comparator>, toComparator(str));
        } else if (isDigit(ch)) {
            std::string str = readUntil(input, isSeparator);
            if (!isNumber(str)) {
                // TODO: throw here
            }
            tokens.emplace_back(token_type_index<TokenType::number>, std::stof(str));
        } else if (isOperator(ch)) {
            tokens.emplace_back(token_type_index<TokenType::op>, toOperator(ch));
            input.ignore();
        } else if (isQuote(ch)) {
            input.ignore();
            std::string str = readUntil(input, ch);
            if (input.peek() != peek) {
                // TODO: throw here
            }
            input.ignore();
            if (ch == singleQuote) {
                tokens.emplace_back(token_type_index<TokenType::text>, str);
            } else {
                tokens.emplace_back(token_type_index<TokenType::identifier>, str);
            }
        } else {
            // TODO: throw here
        }
    }
    return TokenIterator(std::move(tokens));
}

TokenIterator::TokenIterator(std::vector<Token>&& tokens)
    : tokens_(std::move(tokens)) {}

bool TokenIterator::hasNext() const noexcept {
    return remaining() > 0;
}

const Token& TokenIterator::next() {
    return tokens_[i_++];
}

std::size_t TokenIterator::remaining() const noexcept {
    return i_ < tokens_.size() ? tokens_.size() - i_ : 0;
}

const Token& TokenIterator::peek() const {
    return tokens_[i_];
}

bool TokenIterator::skipIf(const TokenPredicate& pred) {
    if (hasNext() && pred(peek())) {
        ++i_;
        return true;
    }
    return false;
}

bool TokenIterator::skipIf(Keyword keyword) {
    return skipIf([keyword](const Token& token) {
        return token.is<TokenType::keyword>(keyword);
    });
}

bool TokenIterator::skipIf(Punctuation punctuation) {
    return skipIf([punctuation](const Token& token) {
        return token.is<TokenType::punctuation>(punctuation);
    });
}

}  // namespace ursql
