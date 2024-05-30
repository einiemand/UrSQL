#include "parser/Parser.hpp"

#include "exception/UserError.hpp"
#include "parser/TokenStream.hpp"
#include "statement/CreateTableStatement.hpp"
#include "statement/DBStatement.hpp"
#include "statement/DropTableStatement.hpp"
#include "statement/InsertIntoTableStatement.hpp"

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
        return CreateDBStatement::parse(ts);
    }
    if (ts.skipIf(Keyword::table_kw)) {
        return CreateTableStatement::parse(ts);
    }
    URSQL_THROW_NORMAL(UnknownCommand, ts);
}

std::unique_ptr<Statement> parseDropStatement(TokenStream& ts) {
    if (ts.skipIf(Keyword::database_kw)) {
        return DropDBStatement::parse(ts);
    }
    if (ts.skipIf(Keyword::table_kw)) {
        return DropTableStatement::parse(ts);
    }
    URSQL_THROW_NORMAL(UnknownCommand, ts);
}

std::unique_ptr<Statement> parseShowStatement(TokenStream& ts) {
    if (ts.skipIf(Keyword::databases_kw)) {
        return ShowDBStatement::parse(ts);
    }
    if (ts.skipIf(Keyword::tables_kw)) {
        return ShowTablesStatement::parse(ts);
    }
    URSQL_THROW_NORMAL(UnknownCommand, ts);
}

std::unique_ptr<Statement> parseDescStatement(TokenStream& ts) {
    if (ts.skipIf(Keyword::database_kw)) {
        return DescDBStatement::parse(ts);
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
        return UseDBStatement::parse(ts);
    }
    if (ts.skipIf(Keyword::show_kw)) {
        return parseShowStatement(ts);
    }
    if (ts.skipIf(Keyword::describe_kw) || ts.skipIf(Keyword::desc_kw)) {
        return parseDescStatement(ts);
    }
    if (ts.skipIf(Keyword::insert_kw)) {
        return InsertIntoTableStatement::parse(ts);
    }
    URSQL_THROW_NORMAL(UnknownCommand, ts);
}

std::string parseNextIdentifier(TokenStream& ts) {
    URSQL_EXPECT(ts.hasNext(), MissingInput, "identifier");
    auto& token = ts.next();
    URSQL_EXPECT(token.getType() == TokenType::identifier, UnexpectedInput,
                 std::format("token type should be identifier but was {}",
                             token.getType()));
    return token.get<TokenType::identifier>();
}

std::string parseNextIdentifierAsLast(TokenStream& ts) {
    std::string id = parseNextIdentifier(ts);
    URSQL_EXPECT(!ts.hasNext(), RedundantInput, ts);
    return id;
}

}  // namespace ursql::parser
