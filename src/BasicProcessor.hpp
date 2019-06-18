#pragma once
#ifndef BASICPROCESSOR_HPP
#define BASICPROCESSOR_HPP
#include "Interpreter.hpp"

namespace UrSQL {

class BasicProcessor : public Interpreter {
public:
	BasicProcessor(Interpreter* anInterpreter = nullptr);
	~BasicProcessor() = default;

	std::unique_ptr<Statement> getStatement(Tokenizer& aTokenizer) override;

};

}

#endif