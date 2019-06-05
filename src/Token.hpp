#pragma once
#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <string>
#include "Keyword.hpp"

namespace UrSQL {

enum class TokenType {
	comma, colon, number, operators,
	lparen, rparen, lbracket, rbracket, lbrace, rbrace,
	slash, star, equal, plus, minus, keyword,
	identifier, string, unknown
};

class Token {
public:
	Token();
	Token(TokenType type, std::string data, Keyword keyword = Keyword::unknown_kw);
	~Token() = default;

	Token(const Token&) = default;
	Token(Token&&) noexcept = default;

	Token& operator=(const Token&) = default;
	Token& operator=(Token&&) noexcept = default;

	inline const std::string& get_data() const {
		return m_data;
	}

	inline TokenType get_type() const {
		return m_type;
	}

	inline Keyword get_keyword() const {
		return m_keyword;
	}
private:
	TokenType m_type;
	std::string m_data;
	Keyword m_keyword;
};

} /* UrSQL */

#endif /* Token.hpp */