#include "token/OperatorToken.hpp"

namespace ursql {

OperatorToken::OperatorToken(Operator op) : Token(TokenType::op), op_(op) {}

Operator OperatorToken::getOperator() const noexcept {
    return op_;
}

}  // namespace ursql
