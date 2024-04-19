#pragma once

#include "Token.hpp"

namespace ursql {

class NumberToken : public Token {
public:
    explicit NumberToken(float number);
    ~NumberToken() override = default;

    [[nodiscard]] float getNumber() const noexcept;

private:
    const float number_;
};

}  // namespace ursql
