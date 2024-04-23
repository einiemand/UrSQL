#pragma once

#include <functional>

#include "Token.hpp"

namespace ursql {

class TokenIterator {
public:
    using TokenPredicate = std::function<bool(const Token&)>;

    static TokenIterator tokenize(std::istream& input);

    explicit TokenIterator(std::vector<Token>&& tokens);
    ~TokenIterator() = default;

    [[nodiscard]] bool hasNext() const noexcept;
    const Token& next();

    [[nodiscard]] std::size_t remaining() const noexcept;
    [[nodiscard]] const Token& peek() const;

    bool skipIf(const TokenPredicate& pred);
    bool skipIf(Keyword keyword);
    bool skipIf(Punctuation punctuation);

private:
    const std::vector<Token> tokens_;
    std::size_t i_ = 0;
};

}  // namespace ursql
