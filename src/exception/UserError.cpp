#include "exception/UserError.hpp"

namespace ursql {

UserError::UserError(const std::string& what) : Exception(what) {}

SyntaxError::SyntaxError(const std::string& what)
    : UserError("syntax error: " + what) {}

}  // namespace ursql
