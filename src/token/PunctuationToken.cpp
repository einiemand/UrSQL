#include "token/PunctuationToken.hpp"

namespace ursql {

PunctuationToken::PunctuationToken(ursql::Punctuation punctuation)
    : Token(TokenType::punctuation),
      punctuation_(punctuation) {}

Punctuation PunctuationToken::getPunctuation() const noexcept {
    return punctuation_;
}

}  // namespace ursql
