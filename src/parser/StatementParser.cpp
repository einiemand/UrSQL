#include "parser/StatementParser.hpp"
#include "parser/TokenStream.hpp"
#include "statement/Statement.hpp"

namespace ursql::parser {

std::unique_ptr<Statement> parse(TokenStream& stream) {
    for (Keyword keyword : { Keyword::help_kw, Keyword::version_kw, Keyword::quit_kw }) {
        if (stream.skipIf(keyword) && !stream.hasNext()) {
            return std::make_unique<BasicStatement>(keyword);
        }
    }
    return nullptr;
}

}


