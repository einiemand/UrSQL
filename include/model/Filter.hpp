#pragma once
#ifndef FILTER_HPP
#define FILTER_HPP
#include "Value.hpp"

namespace UrSQL {

class Tokenizer;
class Row;
class Token;
class Entity;

enum class Comparator : uint8_t {
	e_op,  // =, ==
	ne_op, // !=
	lt_op = 0xF0, // <
	le_op = 0xCC, // <=
	gt_op = 0x0F, // >
	ge_op = 0x33, // >=
	unknown_op = 0xFF
};

static_assert(static_cast<uint8_t>(Comparator::lt_op) + static_cast<uint8_t>(Comparator::gt_op) == 0xFF);
static_assert(static_cast<uint8_t>(Comparator::le_op) + static_cast<uint8_t>(Comparator::ge_op) == 0xFF);

class Expression {
public:
	Expression();
	~Expression() = default;

	Expression(Expression&&) noexcept = default;
	Expression& operator=(const Expression&) = delete;

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

} /* UrSQL */

#endif /* Filter.hpp */