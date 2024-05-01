#pragma once

#include "Exception.hpp"

namespace ursql {

class UserError : public Exception {
public:
    explicit UserError(std::string what);
    ~UserError() override = default;
};

class SyntaxError : public UserError {
public:
    explicit SyntaxError(const std::string& what);
    ~SyntaxError() override = default;
};

}  // namespace ursql
