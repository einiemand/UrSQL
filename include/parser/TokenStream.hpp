#pragma once

#include <functional>

#include "Token.hpp"

namespace ursql {

class TokenStream {
public:
    using TokenPredicate = std::function<bool(const Token&)>;

    explicit TokenStream(std::istream& input);
    ~TokenStream() = default;

    URSQL_DISABLE_COPY(TokenStream);

    [[nodiscard]] bool hasNext() const noexcept;
    const Token& next();

    [[nodiscard]] std::size_t remaining() const noexcept;
    [[nodiscard]] const Token& peek() const;

    bool skipIf(const TokenPredicate& pred);
    bool skipIf(Keyword keyword);
    bool skipIf(Punctuation punctuation);

private:
    const std::vector<Token> tokens_;
    std::size_t i_;

    static std::vector<Token> tokenize(std::istream& input);
};

}  // namespace ursql
