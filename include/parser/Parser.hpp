#pragma once

#include <memory>

#include "Token.hpp"

namespace ursql {

class Statement;
class TokenStream;

namespace parser {

std::unique_ptr<Statement> parse(TokenStream& stream);

}

}
