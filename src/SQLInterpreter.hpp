#pragma once
#ifndef SQLINTERPRETER_HPP
#define SQLINTERPRETER_HPP
#include "Interpreter.hpp"
#include "Attribute.hpp"

namespace UrSQL {

class SQLInterpreter : public Interpreter {
public:
	SQLInterpreter(Interpreter* next);
	~SQLInterpreter() override = default;

	std::unique_ptr<Statement> getStatement(Tokenizer& aTokenizer) override;

	StatusResult createTable(const AttributeList& anAttributeList, const std::string& anEntityName) const;
	StatusResult describeTable(const std::string& anEntityName) const;

private:

};

}

#endif