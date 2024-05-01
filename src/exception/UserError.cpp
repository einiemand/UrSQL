#include "exception/UserError.hpp"

#include <format>

namespace ursql {

UserError::UserError(std::string what) : Exception(std::move(what)) {}

SyntaxError::SyntaxError(const std::string& what)
    : UserError(std::format("syntax error: {}", what)) {}

}  // namespace ursql
