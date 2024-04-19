#include "token/KeywordToken.hpp"

namespace ursql {

KeywordToken::KeywordToken(Keyword keyword)
    : Token(TokenType::keyword),
      keyword_(keyword) {}

Keyword KeywordToken::getKeyword() const noexcept {
    return keyword_;
}

}  // namespace ursql
