#pragma once
#ifndef BASICPROCESSOR_HPP
#define BASICPROCESSOR_HPP
#include "Interpreter.hpp"

namespace UrSQL {

class BasicInterpreter : public Interpreter {
public:
	BasicInterpreter(Interpreter* anInterpreter = nullptr);
	~BasicInterpreter() = default;

	std::unique_ptr<Statement> getStatement(Tokenizer& aTokenizer) override;

};

}

#endif