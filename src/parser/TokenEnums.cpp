#include "parser/TokenEnums.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

#include "common/Macros.hpp"
#include "exception/InternalError.hpp"

namespace ursql {

namespace {

constexpr const char leftParen = '(';
constexpr const char rightParen = ')';
constexpr const char comma = ',';

const std::unordered_map<std::string_view, Keyword> str2keyword{
    { "add", Keyword::add_kw },
    { "and", Keyword::and_kw },
    { "asc", Keyword::asc_kw },
    { "auto_increment", Keyword::auto_increment_kw },
    { "boolean", Keyword::boolean_kw },
    { "by", Keyword::by_kw },
    { "char", Keyword::char_kw },
    { "create", Keyword::create_kw },
    { "database", Keyword::database_kw },
    { "databases", Keyword::databases_kw },
    { "delete", Keyword::delete_kw },
    { "desc", Keyword::desc_kw },
    { "describe", Keyword::describe_kw },
    { "double", Keyword::double_kw },
    { "drop", Keyword::drop_kw },
    { "float", Keyword::float_kw },
    { "from", Keyword::from_kw },
    { "group", Keyword::group_kw },
    { "help", Keyword::help_kw },
    { "in", Keyword::in_kw },
    { "insert", Keyword::insert_kw },
    { "int", Keyword::integer_kw },
    { "integer", Keyword::integer_kw },
    { "into", Keyword::into_kw },
    { "is", Keyword::is_kw },
    { "key", Keyword::key_kw },
    { "not", Keyword::not_kw },
    { "null", Keyword::null_kw },
    { "or", Keyword::or_kw },
    { "order", Keyword::order_kw },
    { "primary", Keyword::primary_kw },
    { "quit", Keyword::quit_kw },
    { "select", Keyword::select_kw },
    { "set", Keyword::set_kw },
    { "show", Keyword::show_kw },
    { "table", Keyword::table_kw },
    { "tables", Keyword::tables_kw },
    { "unique", Keyword::unique_kw },
    { "update", Keyword::update_kw },
    { "use", Keyword::use_kw },
    { "values", Keyword::values_kw },
    { "varchar", Keyword::varchar_kw },
    { "version", Keyword::version_kw },
    { "where", Keyword::where_kw }
};

const std::unordered_map<std::string_view, Comparator> str2Comparator{
    { "=", Comparator::eq },  { "!=", Comparator::ne }, { "<", Comparator::lt },
    { "<=", Comparator::le }, { ">", Comparator::gt },  { ">=", Comparator::ge }
};

}  // namespace

bool strIsComparator(std::string_view str) {
    return str2Comparator.find(str) != std::end(str2Comparator);
}

Comparator toComparator(std::string_view str) {
    auto it = str2Comparator.find(str);
    URSQL_ASSERT(it != std::end(str2Comparator),
                 std::string(str) + " is not a comparator");
    return it->second;
}

bool charIsComparator(char c) {
    return strchr("!<=>", c) != nullptr;
}

bool isKeyword(std::string_view aWord) {
    return str2keyword.find(aWord) != std::end(str2keyword);
}

Keyword toKeyword(std::string_view aWord) {
    auto it = str2keyword.find(aWord);
    return it != std::end(str2keyword) ? it->second : Keyword::unknown_kw;
}

bool isOperator(char c) {
    return strchr("+-*/", c) != nullptr;
}

Operator toOperator(char c) {
    switch (c) {
    case '+':
        return Operator::plus;
    case '-':
        return Operator::minus;
    case '*':
        return Operator::star;
    case '/':
        return Operator::slash;
    default:
        URSQL_UNREACHABLE(std::string(1, c) + " is not an operator");
    }
}

bool isPunctuation(char c) {
    return strchr(",()", c) != nullptr;
}

Punctuation toPunctuation(char c) {
    switch (c) {
    case comma:
        return Punctuation::comma;
    case leftParen:
        return Punctuation::lparen;
    case rightParen:
        return Punctuation::rparen;
    default:
        URSQL_UNREACHABLE(std::string(1, c) + " is not a punctuation");
    }
}

}  // namespace ursql
