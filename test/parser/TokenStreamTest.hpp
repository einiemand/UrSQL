#pragma once

#include <gtest/gtest.h>

#include "exception/UserError.hpp"
#include "parser/TokenStream.hpp"
#include "parser/SQLBlob.hpp"

namespace ursql {

TEST(TokenStream, empty) {
    SQLBlob blob;
    std::istringstream empty(" \t \v \r   \f   \n");
    empty >> blob;
    TokenStream stream = blob.tokenize();
    ASSERT_FALSE(stream.hasNext());
}

TEST(TokenStream, create) {
    SQLBlob blob;
    std::istringstream iss(
      "CrEatE taBLe \t\"some table\" (\"pri key\" prImary key auto_inCrement , "
      "f1 varchAr nOt null DeFauLt 'qwe', somef float\v, inte iNteger) \r \n ");
    iss >> blob;
    TokenStream stream = blob.tokenize();
    ASSERT_EQ(22, stream.remaining());
    ASSERT_EQ(Keyword::create_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::table_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("some table", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Punctuation::lparen, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ("pri key", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::primary_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::key_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::auto_increment_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::comma, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ("f1", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::varchar_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::not_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::null_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::default_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("qwe", stream.next().get<TokenType::text>());
    ASSERT_EQ(Punctuation::comma, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ("somef", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::float_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::comma, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ("inte", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::integer_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::rparen, stream.next().get<TokenType::punctuation>());
    ASSERT_FALSE(stream.hasNext());
}

TEST(TokenStream, insert) {
    SQLBlob blob;
    std::istringstream iss("InSerT iNTo \"some table\"(field1, field_2, fie3d) "
                           "valuEs \t ('zqdsffa', -3.33, true)  ");
    iss >> blob;
    TokenStream stream = blob.tokenize();
    ASSERT_EQ(19, stream.remaining());
    ASSERT_EQ(Keyword::insert_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::into_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("some table", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Punctuation::lparen, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ("field1", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Punctuation::comma, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ("field_2", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Punctuation::comma, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ("fie3d", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Punctuation::rparen, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ(Keyword::values_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::lparen, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ("zqdsffa", stream.next().get<TokenType::text>());
    ASSERT_EQ(Punctuation::comma, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ(Operator::minus, stream.next().get<TokenType::op>());
    ASSERT_FLOAT_EQ(3.33, stream.next().get<TokenType::number>());
    ASSERT_EQ(Punctuation::comma, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ(Keyword::true_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::rparen, stream.next().get<TokenType::punctuation>());
    ASSERT_FALSE(stream.hasNext());
}

TEST(TokenStream, del) {
    SQLBlob blob;
    std::istringstream iss("DelETE from \"qwe zxc\" where f1 != 'sdf' or "
                           "num\t< 222.543 and \"free form\" =\vfalse\f");
    iss >> blob;
    TokenStream stream = blob.tokenize();
    ASSERT_EQ(15, stream.remaining());
    ASSERT_EQ(Keyword::delete_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::from_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("qwe zxc", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::where_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("f1", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::ne, stream.next().get<TokenType::comparator>());
    ASSERT_EQ("sdf", stream.next().get<TokenType::text>());
    ASSERT_EQ(Keyword::or_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("num", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::lt, stream.next().get<TokenType::comparator>());
    ASSERT_FLOAT_EQ(222.543, stream.next().get<TokenType::number>());
    ASSERT_EQ(Keyword::and_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("free form", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::eq, stream.next().get<TokenType::comparator>());
    ASSERT_EQ(Keyword::false_kw, stream.next().get<TokenType::keyword>());
    ASSERT_FALSE(stream.hasNext());
}

TEST(TokenStream, drop) {
    SQLBlob blob;
    std::istringstream iss("Drop \v \t\f TaBlE \"sOme idEntifier\"");
    iss >> blob;
    TokenStream stream = blob.tokenize();
    ASSERT_EQ(3, stream.remaining());
    ASSERT_EQ(Keyword::drop_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::table_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("sOme idEntifier", stream.next().get<TokenType::identifier>());
    ASSERT_FALSE(stream.hasNext());
}

TEST(TokenStream, truncate) {
    SQLBlob blob;
    std::istringstream iss("tRunCaTE \v \t\f TaBlE \nwHat\r");
    iss >> blob;
    TokenStream stream = blob.tokenize();
    ASSERT_EQ(3, stream.remaining());
    ASSERT_EQ(Keyword::truncate_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::table_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("wHat", stream.next().get<TokenType::identifier>());
    ASSERT_FALSE(stream.hasNext());
}

TEST(TokenStream, select) {
    SQLBlob blob;
    std::istringstream iss(
      "select *\vfROm table1 \t wHere \"some id\"-\r34.5=1 \n and\fjOb='no "
      "job' and (age >5 or address is nOt null) oRdEr by nAme desc");
    iss >> blob;
    TokenStream stream = blob.tokenize();
    ASSERT_EQ(29, stream.remaining());
    ASSERT_EQ(Keyword::select_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Operator::star, stream.next().get<TokenType::op>());
    ASSERT_EQ(Keyword::from_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("table1", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::where_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("some id", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Operator::minus, stream.next().get<TokenType::op>());
    ASSERT_FLOAT_EQ(34.5, stream.next().get<TokenType::number>());
    ASSERT_EQ(Comparator::eq, stream.next().get<TokenType::comparator>());
    ASSERT_FLOAT_EQ(1, stream.next().get<TokenType::number>());
    ASSERT_EQ(Keyword::and_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("jOb", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::eq, stream.next().get<TokenType::comparator>());
    ASSERT_EQ("no job", stream.next().get<TokenType::text>());
    ASSERT_EQ(Keyword::and_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::lparen, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ("age", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Comparator::gt, stream.next().get<TokenType::comparator>());
    ASSERT_FLOAT_EQ(5, stream.next().get<TokenType::number>());
    ASSERT_EQ(Keyword::or_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("address", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::is_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::not_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::null_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Punctuation::rparen, stream.next().get<TokenType::punctuation>());
    ASSERT_EQ(Keyword::order_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::by_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("nAme", stream.next().get<TokenType::identifier>());
    ASSERT_EQ(Keyword::desc_kw, stream.next().get<TokenType::keyword>());
    ASSERT_FALSE(stream.hasNext());
}

TEST(TokenStream, describe) {
    SQLBlob blob;
    std::istringstream iss(
      "DeSc descRibE datAbAse taBlE \"what iDentifier\" iden");
    iss >> blob;
    TokenStream stream = blob.tokenize();
    ASSERT_EQ(6, stream.remaining());
    ASSERT_EQ(Keyword::desc_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::describe_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::database_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ(Keyword::table_kw, stream.next().get<TokenType::keyword>());
    ASSERT_EQ("what iDentifier", stream.next().get<TokenType::identifier>());
    ASSERT_EQ("iden", stream.next().get<TokenType::identifier>());
    ASSERT_FALSE(stream.hasNext());
}

TEST(TokenStream, comp) {
    for (std::string compStr : { "!=", "=", "<", "<=", ">", ">=" }) {
        SQLBlob blob;
        std::istringstream iss(compStr);
        iss >> blob;
        TokenStream stream = blob.tokenize();
        ASSERT_EQ(1, stream.remaining());
        ASSERT_EQ(TokenType::comparator, stream.next().getType());
    }
    for (std::string compStr : { "!==", "<<", ">==", "=!", "><", "==" }) {
        SQLBlob blob;
        std::istringstream iss(compStr);
        iss >> blob;
        ASSERT_THROW((void) blob.tokenize(), SyntaxError);
    }
}

TEST(TokenStream, number) {
    for (std::string numStr :
         { "152736.67123", "0", "0.0000", "00000.123400112", "001739" })
    {
        SQLBlob blob;
        std::istringstream iss(numStr);
        iss >> blob;
        TokenStream stream = blob.tokenize();
        ASSERT_EQ(1, stream.remaining());
        ASSERT_EQ(TokenType::number, stream.next().getType());
    }
    for (std::string numStr :
         { "0.000.0", "123a", "0789?", "921.123.", "897!2" })
    {
        SQLBlob blob;
        std::istringstream iss(numStr);
        iss >> blob;
        ASSERT_THROW((void) blob.tokenize(), SyntaxError);
    }
}

TEST(TokenStream, quote) {
    for (std::string str : { "\"asdsafa\"", "\"hfoa\tndfoa   qhrq\"" }) {
        SQLBlob blob;
        std::istringstream iss(str);
        iss >> blob;
        TokenStream stream = blob.tokenize();
        ASSERT_EQ(1, stream.remaining());
        ASSERT_EQ(TokenType::identifier, stream.next().getType());
    }

    for (std::string str : { "'afhwoh21893dafsif'", "'asdnfo\t\f\v128*(&('" }) {
        SQLBlob blob;
        std::istringstream iss(str);
        iss >> blob;
        TokenStream stream = blob.tokenize();
        ASSERT_EQ(1, stream.remaining());
        ASSERT_EQ(TokenType::text, stream.next().getType());
    }

    for (std::string str :
         { "'abc\"", "\"abc'", "'abc", "\"abc", "abc'", "abc\"" })
    {
        SQLBlob blob;
        std::istringstream iss(str);
        iss >> blob;
        ASSERT_THROW((void) blob.tokenize(), SyntaxError);
    }
}

TEST(TokenStream, unknown) {
    for (std::string str : { "!", "@", "#", "$", "%", "^", "&", "_" }) {
        SQLBlob blob;
        std::istringstream iss(str);
        iss >> blob;
        ASSERT_THROW((void) blob.tokenize(), SyntaxError);
    }
}

}  // namespace ursql
