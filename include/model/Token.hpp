#pragma once

#include <string>

#include "Keyword.hpp"
#include "common/Macros.hpp"

namespace UrSQL {

enum class TokenType {
    comma,
    colon,
    number,
    comparator,
    lparen,
    rparen,
    lbracket,
    rbracket,
    lbrace,
    rbrace,
    slash,
    star,
    equal,
    plus,
    minus,
    keyword,
    identifier,
    string,
    unknown
};

class Token {
public:
    Token();
    Token(TokenType type, std::string data,
          Keyword keyword = Keyword::unknown_kw);
    ~Token() = default;

    URSQL_DEFAULT_COPY(Token);
    URSQL_DEFAULT_MOVE(Token);

    inline const std::string& getData() const {
        return m_Data;
    }

    inline TokenType getType() const {
        return m_type;
    }

    inline Keyword getKeyword() const {
        return m_keyword;
    }

    inline bool isValue() const {
        return getType() == TokenType::string || getType() == TokenType::number;
    }

private:
    TokenType m_type;
    std::string m_Data;
    Keyword m_keyword;
};

}  // namespace UrSQL
