#pragma once

#include <memory>

#include "TokenStream.hpp"
#include "exception/UserError.hpp"

namespace ursql {

class Statement;
class TokenStream;

namespace parser {

std::unique_ptr<Statement> parse(TokenStream& ts);

std::string parseNextIdentifier(TokenStream& ts);
std::string parseNextIdentifierAsLast(TokenStream& ts);

template<typename F>
auto parseCommaSeparated(TokenStream& ts, F&& func) {
    std::vector<std::invoke_result_t<F, TokenStream&>> ret;
    do {
        ret.push_back(func(ts));
    } while (ts.skipIf(Punctuation::comma));
    return ret;
}

}  // namespace parser

}  // namespace ursql
