#pragma once

#include <functional>
#include <istream>
#include <vector>

#include "Token.hpp"
#include "common/Error.hpp"

namespace ursql {

class Tokenizer {
public:
    using TokenizeCondition = std::function<bool(char)>;
    using SkipCondition = std::function<bool(const Token&)>;

    explicit Tokenizer(std::istream& anInput);
    ~Tokenizer() = default;

    URSQL_DISABLE_COPY(Tokenizer);

    StatusResult tokenize();

private:
    std::istream& m_input;

    std::string _readWhile(TokenizeCondition aCondition);
    std::string _readUntil(TokenizeCondition aCondition);

    inline std::string _readUntil(char aChar) {
        return _readUntil([aChar](char ch) -> bool {
            return ch == aChar;
        });
    }
};

}  // namespace ursql
