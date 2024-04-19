#pragma once

#include <string>

#include "common/Macros.hpp"
#include "model/Keyword.hpp"

namespace ursql {

enum class TokenType {
    identifier,
    keyword,
    number,
    op,
    punctuation,
    text
    //    comma,
    //    colon,
    //    number,
    //    comparator,
    //    lparen,
    //    rparen,
    //    lbracket,
    //    rbracket,
    //    lbrace,
    //    rbrace,
    //    slash,
    //    star,
    //    equal,
    //    plus,
    //    minus,
    //    keyword,
    //    identifier,
    //    string,
    //    unknown
};

class Token {
public:
    explicit Token(TokenType type);
    virtual ~Token() = 0;

    URSQL_DISABLE_COPY(Token);

    [[nodiscard]] TokenType getType() const noexcept;

private:
    TokenType type_;
};

}  // namespace ursql
