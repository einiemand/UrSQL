#pragma once
#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include "common/Error.hpp"
#include "model/Statement.hpp"
#include <memory>

namespace UrSQL {

class Tokenizer;
class Database;

class Interpreter {
public:
	Interpreter(Interpreter* anInterpreter) :
		m_next(anInterpreter)
	{
	}

	Interpreter(const Interpreter&) = delete;
	Interpreter& operator=(const Interpreter&) = delete;

	virtual std::unique_ptr<Statement> getStatement(Tokenizer& aTokenizer) = 0;
	virtual ~Interpreter() = default;

	StatusResult processInput(Tokenizer& aTokenizer) {
		if (auto theStatement = getStatement(aTokenizer)) {
			return theStatement->run();
		}

		return m_next ? m_next->processInput(aTokenizer) :
			StatusResult(Error::unknown_command, "Enter 'help' to get familiar with this clumsy DB engine");
	}

	virtual Database* getActiveDatabase() const {
		return m_next ? m_next->getActiveDatabase() : nullptr;
	}
protected:
	Interpreter* m_next;
};

}

#endif