#pragma once

#include <memory>

#include "Token.hpp"
#include "exception/UserError.hpp"

namespace ursql {

class Statement;
class TokenStream;

namespace parser {

std::unique_ptr<Statement> parse(TokenStream& ts);

std::string parseNextIdentifier(TokenStream& ts);
std::string parseNextIdentifierAsLast(TokenStream& ts);

}

}  // namespace ursql
