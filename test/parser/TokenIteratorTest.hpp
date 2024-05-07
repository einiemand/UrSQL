#pragma once

#include <gtest/gtest.h>

#include "exception/UserError.hpp"
#include "parser/TokenIterator.hpp"

namespace ursql {

TEST(TokenIterator, empty) {
    std::istringstream empty(" \t \v \r   \f   \n");
    TokenIterator it = TokenIterator::tokenize(empty);
    ASSERT_FALSE(it.hasNext());
}

TEST(TokenIterator, create) {
    std::istringstream iss(
      "CrEatE taBLe \t\"some table\" (\"pri key\" prImary key auto_inCrement , "
      "f1 varchAr nOt null DeFauLt 'qwe', somef float\v, inte iNteger) \r \n ");
    TokenIterator it = TokenIterator::tokenize(iss);
    ASSERT_EQ(22, it.remaining());
    ASSERT_EQ(Keyword::create_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::table_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("some table", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Punctuation::lparen, it.next().get<TokenType::punctuation>());
    ASSERT_EQ("pri key", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::primary_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::key_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::auto_increment_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::comma, it.next().get<TokenType::punctuation>());
    ASSERT_EQ("f1", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::varchar_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::not_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::null_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::default_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("qwe", it.next().get<TokenType::text>());
    ASSERT_EQ(Punctuation::comma, it.next().get<TokenType::punctuation>());
    ASSERT_EQ("somef", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::float_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::comma, it.next().get<TokenType::punctuation>());
    ASSERT_EQ("inte", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::integer_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::rparen, it.next().get<TokenType::punctuation>());
    ASSERT_FALSE(it.hasNext());
}

TEST(TokenIterator, insert) {
    std::istringstream iss("InSerT iNTo \"some table\"(field1, field_2, fie3d) "
                           "valuEs \t ('zqdsffa', -3.33, true)  ");
    TokenIterator it = TokenIterator::tokenize(iss);
    ASSERT_EQ(19, it.remaining());
    ASSERT_EQ(Keyword::insert_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::into_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("some table", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Punctuation::lparen, it.next().get<TokenType::punctuation>());
    ASSERT_EQ("field1", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Punctuation::comma, it.next().get<TokenType::punctuation>());
    ASSERT_EQ("field_2", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Punctuation::comma, it.next().get<TokenType::punctuation>());
    ASSERT_EQ("fie3d", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Punctuation::rparen, it.next().get<TokenType::punctuation>());
    ASSERT_EQ(Keyword::values_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::lparen, it.next().get<TokenType::punctuation>());
    ASSERT_EQ("zqdsffa", it.next().get<TokenType::text>());
    ASSERT_EQ(Punctuation::comma, it.next().get<TokenType::punctuation>());
    ASSERT_EQ(Operator::minus, it.next().get<TokenType::op>());
    ASSERT_FLOAT_EQ(3.33, it.next().get<TokenType::number>());
    ASSERT_EQ(Punctuation::comma, it.next().get<TokenType::punctuation>());
    ASSERT_EQ(Keyword::true_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::rparen, it.next().get<TokenType::punctuation>());
    ASSERT_FALSE(it.hasNext());
}

TEST(TokenIterator, del) {
    std::istringstream iss("DelETE from \"qwe zxc\" where f1 != 'sdf' or "
                           "num\t< 222.543 and \"free form\" =\vfalse\f");
    TokenIterator it = TokenIterator::tokenize(iss);
    ASSERT_EQ(15, it.remaining());
    ASSERT_EQ(Keyword::delete_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::from_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("qwe zxc", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::where_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("f1", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::ne, it.next().get<TokenType::comparator>());
    ASSERT_EQ("sdf", it.next().get<TokenType::text>());
    ASSERT_EQ(Keyword::or_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("num", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::lt, it.next().get<TokenType::comparator>());
    ASSERT_FLOAT_EQ(222.543, it.next().get<TokenType::number>());
    ASSERT_EQ(Keyword::and_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("free form", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::eq, it.next().get<TokenType::comparator>());
    ASSERT_EQ(Keyword::false_kw, it.next().get<TokenType::keyword>());
    ASSERT_FALSE(it.hasNext());
}

TEST(TokenIterator, drop) {
    std::istringstream iss("Drop \v \t\f TaBlE \"sOme idEntifier\"");
    TokenIterator it = TokenIterator::tokenize(iss);
    ASSERT_EQ(3, it.remaining());
    ASSERT_EQ(Keyword::drop_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::table_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("sOme idEntifier", it.next().get<TokenType::identifier>());
    ASSERT_FALSE(it.hasNext());
}

TEST(TokenIterator, truncate) {
    std::istringstream iss("tRunCaTE \v \t\f TaBlE \nwHat\r");
    TokenIterator it = TokenIterator::tokenize(iss);
    ASSERT_EQ(3, it.remaining());
    ASSERT_EQ(Keyword::truncate_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::table_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("wHat", it.next().get<TokenType::identifier>());
    ASSERT_FALSE(it.hasNext());
}

TEST(TokenIterator, select) {
    std::istringstream iss(
      "select *\vfROm table1 \t wHere \"some id\"-\r34.5=1 \n and\fjOb='no "
      "job' and (age >5 or address is nOt null) oRdEr by nAme desc");
    TokenIterator it = TokenIterator::tokenize(iss);
    ASSERT_EQ(29, it.remaining());
    ASSERT_EQ(Keyword::select_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Operator::star, it.next().get<TokenType::op>());
    ASSERT_EQ(Keyword::from_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("table1", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::where_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("some id", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Operator::minus, it.next().get<TokenType::op>());
    ASSERT_FLOAT_EQ(34.5, it.next().get<TokenType::number>());
    ASSERT_EQ(Comparator::eq, it.next().get<TokenType::comparator>());
    ASSERT_FLOAT_EQ(1, it.next().get<TokenType::number>());
    ASSERT_EQ(Keyword::and_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("jOb", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::eq, it.next().get<TokenType::comparator>());
    ASSERT_EQ("no job", it.next().get<TokenType::text>());
    ASSERT_EQ(Keyword::and_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::lparen, it.next().get<TokenType::punctuation>());
    ASSERT_EQ("age", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::gt, it.next().get<TokenType::comparator>());
    ASSERT_FLOAT_EQ(5, it.next().get<TokenType::number>());
    ASSERT_EQ(Keyword::or_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("address", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::is_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::not_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::null_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::rparen, it.next().get<TokenType::punctuation>());
    ASSERT_EQ(Keyword::order_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::by_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("nAme", it.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::desc_kw, it.next().get<TokenType::keyword>());
    ASSERT_FALSE(it.hasNext());
}

TEST(TokenIterator, describe) {
    std::istringstream iss(
      "DeSc descRibE datAbAse taBlE \"what iDentifier\" iden");
    TokenIterator it = TokenIterator::tokenize(iss);
    ASSERT_EQ(6, it.remaining());
    ASSERT_EQ(Keyword::desc_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::describe_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::database_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::table_kw, it.next().get<TokenType::keyword>());
    ASSERT_EQ("what iDentifier", it.next().get<TokenType::identifier>());
    ASSERT_EQ("iden", it.next().get<TokenType::identifier>());
    ASSERT_FALSE(it.hasNext());
}

TEST(TokenIterator, comp) {
    for (std::string compStr : { "!=", "=", "<", "<=", ">", ">=" }) {
        std::istringstream iss(compStr);
        TokenIterator it = TokenIterator::tokenize(iss);
        ASSERT_EQ(1, it.remaining());
        ASSERT_EQ(TokenType::comparator, it.next().getType());
    }
    for (std::string compStr : { "!==", "<<", ">==", "=!", "><", "==" }) {
        std::istringstream iss(compStr);
        ASSERT_THROW(TokenIterator::tokenize(iss), SyntaxError);
    }
}

TEST(TokenIterator, number) {
    for (std::string numStr :
         { "152736.67123", "0", "0.0000", "00000.123400112", "001739" })
    {
        std::istringstream iss(numStr);
        TokenIterator it = TokenIterator::tokenize(iss);
        ASSERT_EQ(1, it.remaining());
        ASSERT_EQ(TokenType::number, it.next().getType());
    }
    for (std::string numStr :
         { "0.000.0", "123a", "0789?", "921.123.", "897!2" })
    {
        std::istringstream iss(numStr);
        ASSERT_THROW(TokenIterator::tokenize(iss), SyntaxError);
    }
}

TEST(TokenIterator, quote) {
    for (std::string str : { "\"asdsafa\"", "\"hfoa\tndfoa   qhrq\"" }) {
        std::istringstream iss(str);
        TokenIterator it = TokenIterator::tokenize(iss);
        ASSERT_EQ(1, it.remaining());
        ASSERT_EQ(TokenType::identifier, it.next().getType());
    }

    for (std::string str : { "'afhwoh21893dafsif'", "'asdnfo\t\f\v128*(&('" }) {
        std::istringstream iss(str);
        TokenIterator it = TokenIterator::tokenize(iss);
        ASSERT_EQ(1, it.remaining());
        ASSERT_EQ(TokenType::text, it.next().getType());
    }

    for (std::string str :
         { "'abc\"", "\"abc'", "'abc", "\"abc", "abc'", "abc\"" })
    {
        std::istringstream iss(str);
        ASSERT_THROW(TokenIterator::tokenize(iss), SyntaxError);
    }
}

TEST(TokenIterator, unknown) {
    for (std::string str : { "!", "@", "#", "$", "%", "^", "&", "_" }) {
        std::istringstream iss(str);
        ASSERT_THROW(TokenIterator::tokenize(iss), SyntaxError);
    }
}

}  // namespace ursql
