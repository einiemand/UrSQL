#include <gtest/gtest.h>
#include "parser/Token.hpp"

namespace ursql {

TEST(TokenTest, identifier) {
    Token token(token_type_index<TokenType::identifier>, "id1");
    ASSERT_EQ(TokenType::identifier, token.getType());
    ASSERT_EQ("id1", token.get<TokenType::identifier>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::keyword>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::number>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::op>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::punctuation>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::text>());
}

TEST(TokenTest, keyword) {
    Token token(token_type_index<TokenType::keyword>, Keyword::select_kw);
    ASSERT_EQ(TokenType::keyword, token.getType());
    ASSERT_EQ(nullptr, token.getIf<TokenType::identifier>());
    ASSERT_EQ(Keyword::select_kw, token.get<TokenType::keyword>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::number>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::op>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::punctuation>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::text>());
}

TEST(TokenTest, number) {
    float num = 1123.12321;
    Token token(token_type_index<TokenType::number>, num);
    ASSERT_EQ(TokenType::number, token.getType());
    ASSERT_EQ(nullptr, token.getIf<TokenType::identifier>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::keyword>());
    ASSERT_EQ(num, token.get<TokenType::number>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::op>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::punctuation>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::text>());
}

TEST(TokenTest, op) {
    Token token(token_type_index<TokenType::op>, Operator::plus);
    ASSERT_EQ(TokenType::op, token.getType());
    ASSERT_EQ(nullptr, token.getIf<TokenType::identifier>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::keyword>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::number>());
    ASSERT_EQ(Operator::plus, token.get<TokenType::op>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::punctuation>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::text>());
}

TEST(TokenTest, punctuation) {
    Token token(token_type_index<TokenType::punctuation>, Punctuation::lparen);
    ASSERT_EQ(TokenType::punctuation, token.getType());
    ASSERT_EQ(nullptr, token.getIf<TokenType::identifier>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::keyword>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::number>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::op>());
    ASSERT_EQ(Punctuation::lparen, token.get<TokenType::punctuation>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::text>());
}

TEST(TokenTest, text) {
    constexpr const char* const str = "agfnao";
    Token token(token_type_index<TokenType::text>, str);
    ASSERT_EQ(TokenType::text, token.getType());
    ASSERT_EQ(nullptr, token.getIf<TokenType::identifier>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::keyword>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::number>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::op>());
    ASSERT_EQ(nullptr, token.getIf<TokenType::punctuation>());
    ASSERT_EQ(str, token.get<TokenType::text>());
}

}
