#include "statement/DBStatement.hpp"

#include <format>

#include "controller/DBManager.hpp"
#include "exception/UserError.hpp"
#include "parser/TokenStream.hpp"
#include "view/TabularView.hpp"
#include "view/TextView.hpp"

namespace ursql {

DBStatement::DBStatement(std::string dbName)
    : Statement(),
      dbName_(std::move(dbName)) {}

CreateDBStatement::CreateDBStatement(std::string dbName)
    : DBStatement(std::move(dbName)) {}

ExecuteResult CreateDBStatement::run(DBManager& dbManager) const {
    dbManager.createDatabase(dbName_);
    return { std::make_unique<TextView>(
               std::format("Database '{}' created", dbName_)),
             false };
}

DropDBStatement::DropDBStatement(std::string dbName)
    : DBStatement(std::move(dbName)) {}

ExecuteResult DropDBStatement::run(DBManager& dbManager) const {
    dbManager.dropDatabase(dbName_);
    return { std::make_unique<TextView>(
               std::format("Database '{}' dropped", dbName_)),
             false };
}

UseDBStatement::UseDBStatement(std::string dbName)
    : DBStatement(std::move(dbName)) {}

ExecuteResult UseDBStatement::run(DBManager& dbManager) const {
    dbManager.useDatabase(dbName_);
    return { std::make_unique<TextView>("Database changed"), false };
}

class ShowDBView : public TabularView {
public:
    explicit ShowDBView(const std::vector<std::string>& dbNames)
        : TabularView({ "Database" }, _dbNames2Rows(dbNames)) {}

    ~ShowDBView() override = default;

private:
    static std::vector<std::vector<Value>> _dbNames2Rows(
      const std::vector<std::string>& dbNames) {
        std::vector<std::vector<Value>> rows;
        rows.reserve(dbNames.size());
        for (auto& dbName : dbNames) {
            rows.push_back({ Value(dbName) });
        }
        return rows;
    }
};

ExecuteResult ShowDBStatement::run(DBManager&) const {
    std::vector<std::string> dbNames = DBManager::getDatabaseNames();
    return { std::make_unique<ShowDBView>(dbNames), false };
}

namespace parser {

namespace {

std::string parseDatabaseName(TokenStream& ts) {
    URSQL_EXPECT(ts.hasNext(), MissingInput, "database name");
    auto& token = ts.next();
    URSQL_EXPECT(token.getType() == TokenType::identifier, UnexpectedInput,
                 "database name should be an identifier");
    return token.get<TokenType::identifier>();
}

std::string parseDatabaseNameAsLast(TokenStream& ts) {
    std::string dbName = parseDatabaseName(ts);
    URSQL_EXPECT(!ts.hasNext(), RedundantInput, ts);
    return dbName;
}

}  // namespace

std::unique_ptr<CreateDBStatement> parseCreateDBStatement(TokenStream& ts) {
    return std::make_unique<CreateDBStatement>(parseDatabaseNameAsLast(ts));
}

std::unique_ptr<DropDBStatement> parseDropDBStatement(TokenStream& ts) {
    return std::make_unique<DropDBStatement>(parseDatabaseNameAsLast(ts));
}

std::unique_ptr<UseDBStatement> parseUseDBStatement(TokenStream& ts) {
    return std::make_unique<UseDBStatement>(parseDatabaseNameAsLast(ts));
}

std::unique_ptr<ShowDBStatement> parseShowDBStatement(TokenStream& ts) {
    URSQL_EXPECT(!ts.hasNext(), RedundantInput, ts);
    return std::make_unique<ShowDBStatement>();
}

}  // namespace parser

}  // namespace ursql
