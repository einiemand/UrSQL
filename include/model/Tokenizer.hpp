#pragma once

#include <functional>
#include <istream>
#include <vector>

#include "Token.hpp"
#include "common/Error.hpp"

namespace UrSQL {

class Tokenizer {
public:
    using TokenizeCondition = std::function<bool(char)>;
    using SkipCondition = std::function<bool(const Token&)>;

    explicit Tokenizer(std::istream& anInput);
    ~Tokenizer() = default;

    URSQL_DISABLE_COPY(Tokenizer);

    inline bool more() const {
        return m_index < m_tokens.size();
    }

    inline size_type remaining() const {
        return m_tokens.size() - m_index;
    }

    const Token& peek(size_type anOffset = 0);
    const Token& get();
    bool next(size_type anOffset = 1);

    inline bool skipIf(TokenType aType) {
        return skipIf([aType](const Token& aToken) {
            return aToken.getType() == aType;
        });
    }

    inline bool skipIf(Keyword aKeyword) {
        return skipIf([aKeyword](const Token& aToken) {
            return aToken.getKeyword() == aKeyword;
        });
    }

    bool skipIf(SkipCondition aCondition);

    StatusResult tokenize();

private:
    std::istream& m_input;
    std::vector<Token> m_tokens;
    size_type m_index;

    std::string _readWhile(TokenizeCondition aCondition);
    std::string _readUntil(TokenizeCondition aCondition);
    inline std::string _readUntil(char aChar) {
        return _readUntil([aChar](char ch) -> bool {
            return ch == aChar;
        });
    }
};

}  // namespace UrSQL
