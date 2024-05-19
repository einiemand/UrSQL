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
    explicit SyntaxError(std::string_view what);
    ~SyntaxError() override = default;
};

class TokenStream;

class UnknownCommand : public UserError {
public:
    explicit UnknownCommand(const TokenStream& ts);
    ~UnknownCommand() override = default;
};

class AlreadyExists : public UserError {
public:
    explicit AlreadyExists(std::string_view what);
    ~AlreadyExists() override = default;
};

class DoesNotExist : public UserError {
public:
    explicit DoesNotExist(std::string_view what);
    ~DoesNotExist() override = default;
};

class MissingInput : public UserError {
public:
    explicit MissingInput(std::string_view what);
    ~MissingInput() override = default;
};

class UnexpectedInput : public UserError {
public:
    explicit UnexpectedInput(std::string_view what);
    ~UnexpectedInput() override = default;
};

}  // namespace ursql
