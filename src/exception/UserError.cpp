#include "exception/UserError.hpp"

#include <format>

namespace ursql {

UserError::UserError(const std::string& what) : Exception(what) {}

SyntaxError::SyntaxError(const std::string& what)
    : UserError(std::format("syntax error: {}", what)) {}

}  // namespace ursql
