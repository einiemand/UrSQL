#include "model/Token.hpp"

namespace UrSQL {

Token::Token() : Token(TokenType::unknown, "") {}

Token::Token(TokenType type, std::string data, Keyword keyword)
    : m_type(type),
      m_Data(std::move(data)),
      m_keyword(keyword) {}

}  // namespace UrSQL