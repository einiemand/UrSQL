#pragma once

#include <functional>
#include <sstream>

#include "Token.hpp"

namespace ursql {

class TokenStream {
public:
    using TokenPredicate = std::function<bool(const Token&)>;

    explicit TokenStream(std::vector<Token>&& tokens);
    ~TokenStream() = default;

    URSQL_DISABLE_COPY(TokenStream);

    [[nodiscard]] bool hasNext() const noexcept;
    const Token& next();

    [[nodiscard]] std::size_t remaining() const noexcept;
    [[nodiscard]] const Token& peek() const;

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] std::string remainingToString() const;

    bool skipIf(const TokenPredicate& pred);
    bool skipIf(Keyword keyword);
    bool skipIf(Punctuation punctuation);

private:
    std::vector<Token> tokens_;
    std::size_t i_;

    std::string _toString(std::size_t i) const;
};

}  // namespace ursql
