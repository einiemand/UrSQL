#include "token/Token.hpp"

namespace ursql {

Token::Token(TokenType type) : type_(type) {}

TokenType Token::getType() const noexcept {
    return type_;
}

}  // namespace ursql
