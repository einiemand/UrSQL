#pragma once

#include "Exception.hpp"

namespace ursql {

class InternalError : public Exception {
public:
    explicit InternalError(std::string what);
    ~InternalError() override = default;
};

class FileAccessError : public InternalError {
public:
    explicit FileAccessError(const std::string& what);
    ~FileAccessError() override = default;
};

class FatalError : public InternalError {
public:
    explicit FatalError(std::string what);
    ~FatalError() override = default;
};

class AssertFailure : public FatalError {
public:
    explicit AssertFailure(const std::string& what);
    ~AssertFailure() override = default;
};

class UnreachableReached : public FatalError {
public:
    explicit UnreachableReached(const std::string& what);
    ~UnreachableReached() override = default;
};

}  // namespace ursql
