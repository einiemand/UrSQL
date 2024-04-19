#pragma once

#include "Token.hpp"

namespace ursql {

enum Operator {
    plus,
    minus,
    multiply,
    divide
};

class OperatorToken : public Token {
public:
    explicit OperatorToken(Operator op);
    ~OperatorToken() override = default;

    [[nodiscard]] getOperator() const noexcept;

private:
    const Operator op_;
};

}  // namespace ursql
