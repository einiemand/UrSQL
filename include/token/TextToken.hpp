#pragma once

#include "Token.hpp"

namespace ursql {

class TextToken : public Token {
public:
    explicit TextToken(std::string text);
    ~TextToken() override = default;

    [[nodiscard]] const std::string& getText() const noexcept;

private:
    const std::string text_;
};

}  // namespace ursql
