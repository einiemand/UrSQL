#include "token/NumberToken.hpp"

namespace ursql {

NumberToken::NumberToken(float number)
    : Token(TokenType::number),
      number_(number) {}

float NumberToken::getNumber() const noexcept {
    return number_;
}

}  // namespace ursql
