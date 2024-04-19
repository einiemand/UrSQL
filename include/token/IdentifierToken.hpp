#pragma once

#include "Token.hpp"

namespace ursql {

class IdentifierToken : public Token {
public:
    explicit IdentifierToken(std::string fieldName);
    ~IdentifierToken() override = default;

    [[nodiscard]] const std::string& getFieldName() const noexcept;

private:
    const std::string fieldName_;
};

}  // namespace ursql
