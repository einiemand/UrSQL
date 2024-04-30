#include "exception/InternalError.hpp"

#include <format>

namespace ursql {

InternalError::InternalError(const std::string& what) : Exception(what) {}

AssertFailure::AssertFailure(const std::string& what)
    : InternalError(std::format("assert failure: {}", what)) {}

UnreachableReached::UnreachableReached(const std::string& what)
    : InternalError(std::format("unreachable code reached: {}", what)) {}

}  // namespace ursql
