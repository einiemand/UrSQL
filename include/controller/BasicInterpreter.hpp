#pragma once

#include "Interpreter.hpp"

namespace UrSQL {

class BasicInterpreter : public Interpreter {
public:
    explicit BasicInterpreter(Interpreter* anInterpreter = nullptr);
    ~BasicInterpreter() override = default;

    std::unique_ptr<Statement> getStatement(Tokenizer& aTokenizer) override;
};

}  // namespace UrSQL
