#include "parser/TokenStream.hpp"

namespace ursql {

TokenStream::TokenStream(std::vector<Token>&& tokens) : tokens_(std::move(tokens)), i_(0) {}

bool TokenStream::hasNext() const noexcept {
    return remaining() > 0;
}

const Token& TokenStream::next() {
    return tokens_[i_++];
}

std::size_t TokenStream::remaining() const noexcept {
    return tokens_.size() - i_;
}

const Token& TokenStream::peek() const {
    return tokens_[i_];
}

std::string TokenStream::toString() const {
    if (tokens_.empty()) {
        return {};
    }
    std::string str = tokens_[0].toString();
    for (std::size_t i = 1; i < tokens_.size(); ++i) {
        str += ' ';
        str += tokens_[i].toString();
    }
    return str;
}

bool TokenStream::skipIf(const TokenPredicate& pred) {
    if (hasNext() && pred(peek())) {
        ++i_;
        return true;
    }
    return false;
}

bool TokenStream::skipIf(Keyword keyword) {
    return skipIf([keyword](const Token& token) {
        return token.is<TokenType::keyword>(keyword);
    });
}

bool TokenStream::skipIf(Punctuation punctuation) {
    return skipIf([punctuation](const Token& token) {
        return token.is<TokenType::punctuation>(punctuation);
    });
}

}  // namespace ursql
