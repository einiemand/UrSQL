#include "exception/UserError.hpp"

#include <format>

#include "parser/TokenStream.hpp"

namespace ursql {

UserError::UserError(std::string what) : Exception(std::move(what)) {}

SyntaxError::SyntaxError(std::string_view what)
    : UserError(std::format("syntax error: {}", what)) {}

UnknownCommand::UnknownCommand(const TokenStream& ts)
    : UserError(std::format("unknown command {}", ts.toString())) {}

AlreadyExists::AlreadyExists(std::string_view what)
    : UserError(std::format("already exists: {}", what)) {}

DoesNotExist::DoesNotExist(std::string_view what)
    : UserError(std::format("does not exist: {}", what)) {}

MissingInput::MissingInput(std::string_view what)
    : UserError(std::format("missing input: {}", what)) {}

UnexpectedInput::UnexpectedInput(std::string_view what)
    : UserError(std::format("unexpected input: {}", what)) {}

RedundantInput::RedundantInput(TokenStream& ts)
    : UserError(std::format("redundant input: {}", ts.remainingToString())) {}

NoActiveDB::NoActiveDB() : UserError("No database selected") {}

}  // namespace ursql
