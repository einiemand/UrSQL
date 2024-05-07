#pragma once

#include <string_view>

namespace ursql {

enum class Comparator {
    eq,
    ne,
    lt,
    le,
    gt,
    ge
};

bool strIsComparator(std::string_view str);
Comparator toComparator(std::string_view str);
bool charIsComparator(char c);

enum class Keyword {
    add_kw,
    and_kw,
    asc_kw,
    auto_increment_kw,
    boolean_kw,
    by_kw,
    char_kw,
    create_kw,
    database_kw,
    databases_kw,
    desc_kw,
    default_kw,
    delete_kw,
    describe_kw,
    double_kw,
    drop_kw,
    false_kw,
    float_kw,
    from_kw,
    group_kw,
    help_kw,
    in_kw,
    insert_kw,
    integer_kw,
    into_kw,
    is_kw,
    key_kw,
    not_kw,
    null_kw,
    or_kw,
    order_kw,
    primary_kw,
    quit_kw,
    select_kw,
    set_kw,
    show_kw,
    table_kw,
    tables_kw,
    true_kw,
    truncate_kw,
    unique_kw,
    update_kw,
    use_kw,
    values_kw,
    varchar_kw,
    version_kw,
    where_kw
};

bool isKeyword(std::string_view str);
Keyword toKeyword(std::string_view str);

enum class Operator {
    plus,
    minus,
    star,
    slash
};

bool isOperator(char c);
Operator toOperator(char c);

enum class Punctuation {
    comma,
    lparen,
    rparen
};

bool isPunctuation(char c);
Punctuation toPunctuation(char c);

}  // namespace ursql
