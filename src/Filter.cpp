#pragma once
#include "Filter.hpp"
#include "Tokenizer.hpp"
#include "Row.hpp"
#include "Entity.hpp"
#include <unordered_map>

namespace UrSQL {

static const std::unordered_map<std::string, Comparator> compMap{
	{ "=",Comparator::e_op },
	{ "==",Comparator::e_op },
	{ "!=",Comparator::ne_op },
	{ "<",Comparator::lt_op },
	{ "<=",Comparator::le_op },
	{ ">",Comparator::gt_op },
	{ ">=",Comparator::ge_op}
};

bool isValidComparator(const std::string& aString) {
	return compMap.count(aString) == 1;
}

Comparator string2Comparator(const std::string& aString) {
	if (isValidComparator(aString)) {
		return compMap.at(aString);
	}
	throw std::runtime_error("Check if a string is a comparator first!");
}

void reverseComparator(Comparator& aComparator) {
	switch (aComparator) {
	case Comparator::e_op:
	case Comparator::ne_op:
		break;
	default:
		aComparator = static_cast<Comparator>(0xFF - static_cast<uint8_t>(aComparator));
	}
}

Expression::Expression() :
	m_fieldName(),
	m_comparator(Comparator::unknown_op),
	m_value()
{
}

StatusResult Expression::parse(Tokenizer& aTokenizer) {
	StatusResult theResult(Error::no_error);
	if (aTokenizer.remaining() >= 3) {
		const Token& theLHS = aTokenizer.get();
		if (theResult = _parseComparator(aTokenizer)) {
			const Token& theRHS = aTokenizer.get();
			if (theLHS.getType() == TokenType::identifier &&
				(theRHS.getType() == TokenType::number || theRHS.getType() == TokenType::string)) {
				m_fieldName = theLHS.getData();
				m_value = theRHS.getData();
			}
			else if (theRHS.getType() == TokenType::identifier &&
				(theLHS.getType() == TokenType::number || theLHS.getType() == TokenType::string)) {
				m_fieldName = theRHS.getData();
				m_value = theLHS.getData();
				reverseComparator(m_comparator);
			}
			else {
				theResult.setError(Error::syntax_error, "Operands should be one identifier and one string/number");
			}
		}
	}
	else {
		theResult.setError(Error::syntax_error, "Not enough input to parse Expression");
	}
	return theResult;
}

StatusResult Expression::validate(const Entity& anEntity) const {
	StatusResult theResult(Error::no_error);
	if (anEntity.attributeExistsByName(m_fieldName)) {
		theResult = m_value.become(anEntity.getAttributeByName(m_fieldName).getType());
	}
	else {
		theResult.setError(Error::unknown_attribute, '\'' + m_fieldName + '\'');
	}
	return theResult;
}

bool Expression::match(const Row& aRow) const {
	if (aRow.fieldExists(m_fieldName)) {
		const Value& theValue = aRow.getField(m_fieldName);
		switch (m_comparator) {
		case Comparator::e_op:
			return (theValue == m_value);
		case Comparator::ne_op:
			return (theValue != m_value);
		case Comparator::lt_op:
			return (theValue < m_value);
		case Comparator::le_op:
			return (theValue <= m_value);
		case Comparator::gt_op:
			return (theValue > m_value);
		case Comparator::ge_op:
			return (theValue >= m_value);
		default:
			throw std::runtime_error("Impossible: unknown comparator");
		}
	}
	else {
		throw std::runtime_error("Impossible: m_fieldName doesn't exist in entity");
	}
}

StatusResult Expression::_parseComparator(Tokenizer& aTokenizer) {
	StatusResult theResult(Error::no_error);
	const std::string& theCompString = aTokenizer.get().getData();
	if (isValidComparator(theCompString)) {
		m_comparator = string2Comparator(theCompString);
	}
	else {
		theResult.setError(Error::invalid_comparator, '\'' + theCompString + '\'');
	}
	return theResult;
}

StatusResult Filter::parse(Tokenizer& aTokenizer) {
	StatusResult theResult(Error::no_error);
	if (aTokenizer.more()) {
		do {
			Expression theExpression;
			theResult = theExpression.parse(aTokenizer);
			if (theResult) {
				m_expressions.push_back(std::move(theExpression));
			}
		} while (theResult && aTokenizer.skipIf(
			[this](const Token& aToken)->bool {
				switch (aToken.getKeyword()) {
				case Keyword::and_kw:
					m_relations.push_back(ExpRelation::AND);
					return true;
				case Keyword::or_kw:
					m_relations.push_back(ExpRelation::OR);
					return true;
				default:
					return false;
				}
			}
		));
	}
	else {
		theResult.setError(Error::syntax_error, "Not enough input to parse Filter");
	}
	return theResult;
}

StatusResult Filter::validate(const Entity& anEntity) const {
	StatusResult theResult(Error::no_error);
	for (auto iter = m_expressions.cbegin(); theResult && iter != m_expressions.cend(); ++iter) {
		theResult = iter->validate(anEntity);
	}
	return theResult;
}

bool Filter::match(const Row& aRow) const {
	return false;
}

} /* UrSQL */