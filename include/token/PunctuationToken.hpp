#pragma once

#include "Token.hpp"

namespace ursql {

enum class Punctuation {
    comma,
    lparen,
    rparen
};

class PunctuationToken : public Token {
public:
    explicit PunctuationToken(Punctuation punctuation);
    ~PunctuationToken() override = default;

    [[nodiscard]] Punctuation getPunctuation() const noexcept;

private:
    const Punctuation punctuation_;
};

}  // namespace ursql
