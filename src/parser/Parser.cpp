#include "parser/Parser.hpp"

#include "exception/UserError.hpp"
#include "parser/TokenStream.hpp"
#include "statement/DBStatement.hpp"

namespace ursql::parser {

namespace {

std::unique_ptr<Statement> parseKeywordStatement(TokenStream& ts) {
    if (ts.skipIf(Keyword::help_kw)) {
        return std::make_unique<HelpStatement>();
    }
    if (ts.skipIf(Keyword::version_kw)) {
        return std::make_unique<VersionStatement>();
    }
    if (ts.skipIf(Keyword::quit_kw)) {
        return std::make_unique<QuitStatement>();
    }
    URSQL_THROW_NORMAL(UnknownCommand, ts);
}

std::unique_ptr<Statement> parseCreateStatement(TokenStream& ts) {
    if (ts.skipIf(Keyword::database_kw)) {
        return parseCreateDBStatement(ts);
    }
    URSQL_THROW_NORMAL(UnknownCommand, ts);
}

std::unique_ptr<Statement> parseDropStatement(TokenStream& ts) {
    if (ts.skipIf(Keyword::database_kw)) {
        return parseDropDBStatement(ts);
    }
    URSQL_THROW_NORMAL(UnknownCommand, ts);
}

std::unique_ptr<Statement> parseShowStatement(TokenStream& ts) {
    if (ts.skipIf(Keyword::databases_kw)) {
        return parseShowDBStatement(ts);
    }
    URSQL_THROW_NORMAL(UnknownCommand, ts);
}

}  // namespace

std::unique_ptr<Statement> parse(TokenStream& ts) {
    if (!ts.hasNext()) {
        return std::make_unique<NopStatement>();
    }
    if (ts.remaining() == 1) {
        return parseKeywordStatement(ts);
    }
    if (ts.skipIf(Keyword::create_kw)) {
        return parseCreateStatement(ts);
    }
    if (ts.skipIf(Keyword::drop_kw)) {
        return parseDropStatement(ts);
    }
    if (ts.skipIf(Keyword::use_kw)) {
        return parseUseDBStatement(ts);
    }
    if (ts.skipIf(Keyword::show_kw)) {
        return parseShowStatement(ts);
    }
    URSQL_THROW_NORMAL(UnknownCommand, ts);
}

}  // namespace ursql::parser
