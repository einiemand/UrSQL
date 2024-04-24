#include "exception/InternalError.hpp"

namespace ursql {

InternalError::InternalError(const std::string& what) : Exception(what) {}

AssertFailure::AssertFailure(const std::string& what)
    : InternalError("assert failure: " + what) {}

UnreachableReached::UnreachableReached(const std::string& what)
    : InternalError("unreachable code reached: " + what) {}

}  // namespace ursql
