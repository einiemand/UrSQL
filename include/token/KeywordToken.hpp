#pragma once

#include "Token.hpp"

namespace ursql {

class KeywordToken : public Token {
public:
    explicit KeywordToken(Keyword keyword);
    ~KeywordToken() override = default;

    [[nodiscard]] Keyword getKeyword() const noexcept;

private:
    const Keyword keyword_;
};

}  // namespace ursql
