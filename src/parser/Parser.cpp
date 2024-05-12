#include "parser/Parser.hpp"

#include "exception/UserError.hpp"
#include "parser/TokenStream.hpp"
#include "statement/Statement.hpp"

namespace ursql::parser {

std::unique_ptr<Statement> parse(TokenStream& stream) {
    if (!stream.hasNext()) {
        return std::make_unique<NopStatement>();
    }
    for (Keyword keyword : { Keyword::help_kw, Keyword::version_kw, Keyword::quit_kw }) {
        if (stream.skipIf(keyword) && !stream.hasNext()) {
            return std::make_unique<BasicStatement>(keyword);
        }
    }
    URSQL_THROW_NORMAL(SyntaxError, "unknown command");
}

}


