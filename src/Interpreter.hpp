#pragma once
#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP
#include "Error.hpp"
#include "Statement.hpp"
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

	virtual std::unique_ptr<Statement> get_statement(Tokenizer& aTokenizer) = 0;
	virtual ~Interpreter() = default;

	StatusResult process_input(Tokenizer& aTokenizer) {
		if (auto theStatement = get_statement(aTokenizer)) {
			StatusResult theResult = theStatement->parse();
			if (theResult) {
				theResult = theStatement->validate();
				if (theResult) {
					theResult = theStatement->run();
				}
			}
			return theResult;
		}

		return m_next ? m_next->process_input(aTokenizer) :
			StatusResult(Error::unknown_command, "Enter 'help' to get familiar with this clumsy DB engine.");
	}

	virtual Database* get_active_database() const {
		return m_next ? m_next->get_active_database() : nullptr;
	}
protected:
	Interpreter* m_next;
};

}

#endif