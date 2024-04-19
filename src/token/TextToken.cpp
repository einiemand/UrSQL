#include "token/TextToken.hpp"

namespace ursql {

TextToken::TextToken(std::string text)
    : Token(TokenType::text),
      text_(std::move(text)) {}

const std::string& TextToken::getText() const noexcept {
    return text_;
}

}  // namespace ursql
