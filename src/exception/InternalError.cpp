#include "exception/InternalError.hpp"

#include <format>

namespace ursql {

InternalError::InternalError(std::string what) : Exception(std::move(what)) {}

FileAccessError::FileAccessError(const std::string& what)
    : InternalError(std::format("file access error: {}", what)) {}

FatalError::FatalError(std::string what) : InternalError(std::move(what)) {}

AssertFailure::AssertFailure(const std::string& what)
    : FatalError(std::format("assert failure: {}", what)) {}

UnreachableReached::UnreachableReached(const std::string& what)
    : FatalError(std::format("unreachable code reached: {}", what)) {}

}  // namespace ursql
