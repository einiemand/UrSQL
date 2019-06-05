#include "Token.hpp"

namespace UrSQL {

Token::Token() :
	Token(TokenType::unknown, "")
{
}

Token::Token(TokenType type, std::string data, Keyword keyword) :
	m_type(type),
	m_data(std::move(data)),
	m_keyword(keyword)
{
}

}