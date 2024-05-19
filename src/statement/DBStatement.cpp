#include "statement/DBStatement.hpp"

#include "controller/DBManager.hpp"
#include "exception/UserError.hpp"
#include "view/View.hpp"
#include "parser/TokenStream.hpp"
#include <format>

namespace ursql {

DBStatement::DBStatement(std::string dbName) : Statement(), dbName_(std::move(dbName)) {}

CreateDBStatement::CreateDBStatement(std::string dbName) : DBStatement(std::move(dbName)) {}

ExecuteResult CreateDBStatement::run(DBManager& dbManager) const {
    dbManager.createDatabase(dbName_);
    return { std::make_unique<TextView>(std::format("Database '{}' created", dbName_)), false };
}

DropDBStatement::DropDBStatement(std::string dbName) : DBStatement(std::move(dbName)) {}

ExecuteResult DropDBStatement::run(DBManager& dbManager) const {
    dbManager.dropDatabase(dbName_);
    return { std::make_unique<TextView>(std::format("Database '{}' dropped", dbName_)), false };
}

UseDBStatement::UseDBStatement(std::string dbName) : DBStatement(std::move(dbName)) {}

ExecuteResult UseDBStatement::run(DBManager& dbManager) const {
    dbManager.useDatabase(dbName_);
    return { std::make_unique<TextView>("Database changed"), false };
}

namespace parser {

namespace {

std::string parseDatabaseName(TokenStream& ts) {
    URSQL_EXPECT(ts.hasNext(), MissingInput, "database name");
    auto& token = ts.next();
    URSQL_EXPECT(token.getType() == TokenType::identifier, UnexpectedInput, "database name should be an identifier");
    return token.get<TokenType::identifier>();
}

}

std::unique_ptr<CreateDBStatement> parseCreateDBStatement(TokenStream& ts) {
    return std::make_unique<CreateDBStatement>(parseDatabaseName(ts));
}

std::unique_ptr<DropDBStatement> parseDropDBStatement(TokenStream& ts) {
    return std::make_unique<DropDBStatement>(parseDatabaseName(ts));
}

std::unique_ptr<UseDBStatement> parseUseDBStatement(TokenStream& ts) {
    return std::make_unique<UseDBStatement>(parseDatabaseName(ts));
}

}

}
