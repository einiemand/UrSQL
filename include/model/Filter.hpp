#pragma once

#include "Value.hpp"

namespace ursql {

class Tokenizer;
class Row;
class Token;
class Entity;

enum class Comparator : uint8_t {
    eq_op = 0x00,          // =, ==
    ne_op,                 // !=
    lt_op,                 // <
    le_op,                 // <=
    gt_op = 0xFF - lt_op,  // >
    ge_op = 0xFF - le_op,  // >=
    unknown_op = 0xFF
};

static_assert(static_cast<uint8_t>(Comparator::lt_op) +
                static_cast<uint8_t>(Comparator::gt_op) ==
              0xFF);
static_assert(static_cast<uint8_t>(Comparator::le_op) +
                static_cast<uint8_t>(Comparator::ge_op) ==
              0xFF);

class Expression {
public:
    Expression();
    ~Expression() = default;

    URSQL_DEFAULT_MOVE_CTOR(Expression);
    URSQL_DISABLE_COPY_ASSIGN(Expression);

    StatusResult parse(Tokenizer& aTokenizer);
    StatusResult validate(const Entity& anEntity) const;

    bool match(const Row& aRow) const;

private:
    std::string m_fieldName;
    Comparator m_comparator;
    mutable Value m_value;

    StatusResult _parseComparator(Tokenizer& aTokenizer);
};

using ExpressionList = std::vector<Expression>;

class Filter {
public:
    enum class ExpRelation {
        AND,
        OR
    };
    using ExpRelationList = std::vector<ExpRelation>;

    Filter() = default;
    ~Filter() = default;

    Filter(const Filter&) = delete;
    Filter& operator=(const Filter&) = delete;

    StatusResult parse(Tokenizer& aTokenizer);
    StatusResult validate(const Entity& anEntity) const;

    bool match(const Row& aRow) const;

private:
    ExpressionList m_expressions;
    ExpRelationList m_relations;
};

}  // namespace ursql
