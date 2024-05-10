#pragma once

#include <memory>

namespace ursql {

class Statement;
class TokenStream;

namespace parser {

std::unique_ptr<Statement> parse(TokenStream& stream);

}

}
