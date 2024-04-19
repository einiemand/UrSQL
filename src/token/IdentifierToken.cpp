#include "token/IdentifierToken.hpp"

namespace ursql {

IdentifierToken::IdentifierToken(std::string fieldName)
    : Token(TokenType::identifier),
      fieldName_(std::move(fieldName)) {}

const std::string& IdentifierToken::getFieldName() const noexcept {
    return fieldName_;
}

}  // namespace ursql
