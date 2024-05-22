#include "statement/DBStatement.hpp"

#include <format>

#include "controller/DBManager.hpp"
#include "exception/UserError.hpp"
#include "parser/Parser.hpp"
#include "parser/TokenStream.hpp"
#include "view/RowsAffectedTextView.hpp"
#include "view/TabularView.hpp"

namespace ursql {

DBStatement::DBStatement(std::string dbName)
    : Statement(),
      dbName_(std::move(dbName)) {}

CreateDBStatement::CreateDBStatement(std::string dbName)
    : DBStatement(std::move(dbName)) {}

ExecuteResult CreateDBStatement::run(DBManager& dbManager) const {
    dbManager.createDatabase(dbName_);
    return { std::make_unique<RowsAffectedTextView>(1), false };
}

std::unique_ptr<CreateDBStatement> CreateDBStatement::parse(TokenStream& ts) {
    return std::make_unique<CreateDBStatement>(
      parser::parseNextIdentifierAsLast(ts));
}

DropDBStatement::DropDBStatement(std::string dbName)
    : DBStatement(std::move(dbName)) {}

ExecuteResult DropDBStatement::run(DBManager& dbManager) const {
    dbManager.dropDatabase(dbName_);
    return { std::make_unique<RowsAffectedTextView>(1), false };
}

std::unique_ptr<DropDBStatement> DropDBStatement::parse(TokenStream& ts) {
    return std::make_unique<DropDBStatement>(
      parser::parseNextIdentifierAsLast(ts));
}

UseDBStatement::UseDBStatement(std::string dbName)
    : DBStatement(std::move(dbName)) {}

ExecuteResult UseDBStatement::run(DBManager& dbManager) const {
    dbManager.useDatabase(dbName_);
    return { std::make_unique<TextView>("Database changed"), false };
}

std::unique_ptr<UseDBStatement> UseDBStatement::parse(TokenStream& ts) {
    return std::make_unique<UseDBStatement>(
      parser::parseNextIdentifierAsLast(ts));
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

std::unique_ptr<ShowDBStatement> ShowDBStatement::parse(TokenStream& ts) {
    URSQL_EXPECT(!ts.hasNext(), RedundantInput, ts);
    return std::make_unique<ShowDBStatement>();
}

}  // namespace ursql
