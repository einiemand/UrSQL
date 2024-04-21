#include "parser/Token.hpp"

namespace ursql {

TokenType Token::getType() const noexcept {
    return static_cast<TokenType>(var_.index());
}

bool Token::isKeyword(Keyword keyword) const noexcept {
    return getType() == TokenType::keyword && get<TokenType::keyword>() == keyword;
}

bool Token::isPunctuation(Punctuation punctuation) const noexcept {
    return getType() == TokenType::punctuation && get<TokenType::punctuation>() == punctuation;
}

}  // namespace ursql
