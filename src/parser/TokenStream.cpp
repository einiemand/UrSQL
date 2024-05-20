#include "parser/TokenStream.hpp"

namespace ursql {

TokenStream::TokenStream(std::vector<Token>&& tokens)
    : tokens_(std::move(tokens)),
      i_(0) {}

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
    return _toString(0);
}

std::string TokenStream::remainingToString() const {
    return _toString(i_);
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

std::string TokenStream::_toString(std::size_t i) const {
    if (i >= tokens_.size()) {
        return {};
    }
    std::string str = tokens_[i].toString();
    for (; i < tokens_.size(); ++i) {
        str += ' ';
        str += tokens_[i].toString();
    }
    return str;
}

}  // namespace ursql
