#pragma once

#include "Exception.hpp"

namespace ursql {

class InternalError : public Exception {
public:
    explicit InternalError(std::string what);
    ~InternalError() override = default;
};

class AssertFailure : public InternalError {
public:
    explicit AssertFailure(const std::string& what);
    ~AssertFailure() override = default;
};

class UnreachableReached : public InternalError {
public:
    explicit UnreachableReached(const std::string& what);
    ~UnreachableReached() override = default;
};

}  // namespace ursql
