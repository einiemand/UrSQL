#pragma once
#ifndef SQLINTERPRETER_HPP
#define SQLINTERPRETER_HPP
#include "Interpreter.hpp"
#include "Attribute.hpp"
#include "Row.hpp"

namespace UrSQL {

class Filter;
class Order;

class SQLInterpreter : public Interpreter {
public:
	SQLInterpreter(Interpreter* next);
	~SQLInterpreter() override = default;

	std::unique_ptr<Statement> getStatement(Tokenizer& aTokenizer) override;

	StatusResult createTable(const AttributeList& anAttributeList, const std::string& anEntityName) const;
	StatusResult describeTable(const std::string& anEntityName) const;
	StatusResult dropTable(const std::string& anEntityName) const;
	StatusResult showTables() const;

	StatusResult insertIntoTable(const std::string& anEntityName, const StringList& aFieldNames, const std::vector<StringList>& aValueStrsOfRows) const;
	StatusResult selectFromTable(const std::string& anEntityName, StringList& aFieldNames, const Filter* aFilter, const Order* anOrder) const;
	StatusResult deleteFromTable(const std::string& anEntityName, const Filter* aFilter) const;
	StatusResult updateTable(const std::string& anEntityName, const Row::DataMap& aFieldValues, const Filter* aFilter) const;

private:

};

}

#endif