#pragma once

#include <gtest/gtest.h>
#include "parser/TokenIterator.hpp"

namespace ursql {

TEST(TokenIteratorTest, empty) {
    std::istringstream empty(" \t \v \r   \f   \n");
    TokenIterator it = TokenIterator::tokenize(empty);
    ASSERT_FALSE(it.hasNext());
}

TEST(TokenIteratorTest, select) {
    std::istringstream iss("select *\vfrom table1 \t where \"some id\"-\r34.5=1 \n and\fjob='no job' and (age >5 or address is not null) order by name desc");
    TokenIterator it = TokenIterator::tokenize(iss);
    ASSERT_EQ(29, it.remaining());
    ASSERT_EQ(Keyword::select_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Operator::star, it.next().get<TokenType::op>());
    ASSERT_EQ(Keyword::from_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("table1", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::where_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("some id", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Operator::minus, it.next().get<TokenType::op>());
    ASSERT_EQ(34.5, it.next().get<TokenType::number>());
    ASSERT_EQ(Comparator::eq, it.next().get<TokenType::comparator>());
    ASSERT_EQ(1, it.next().get<TokenType::number>());
    ASSERT_EQ(Keyword::and_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("job", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::eq, it.next().get<TokenType::comparator>());
    ASSERT_EQ("no job", it.next().get<TokenType::text>());
    ASSERT_EQ(Keyword::and_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::lparen, it.next().get<TokenType::punctuation>());
    ASSERT_EQ("age", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::gt, it.next().get<TokenType::comparator>());
    ASSERT_EQ(5, it.next().get<TokenType::number>());
    ASSERT_EQ(Keyword::or_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("address", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::is_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::not_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::null_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::rparen, it.next().get<TokenType::punctuation>());
    ASSERT_EQ(Keyword::order_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::by_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("name", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::desc_kw, it.next().get<TokenType::keyword>());
    ASSERT_FALSE(it.hasNext());
}

}
