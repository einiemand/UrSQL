#include "statement/DropTableStatement.hpp"

#include "controller/DBManager.hpp"
#include "parser/Parser.hpp"
#include "view/RowsAffectedTextView.hpp"

namespace ursql {

DropTableStatement::DropTableStatement(std::vector<std::string> tableNames)
    : MultiTableStatement(std::move(tableNames)) {}

ExecuteResult DropTableStatement::run(DBManager& dbManager) const {
    Database* activeDB = dbManager.getActiveDB();
    URSQL_EXPECT(activeDB, NoActiveDB, );
    activeDB->dropTables(tableNames_);
    return { std::make_unique<RowsAffectedTextView>(tableNames_.size()),
             false };
}

std::unique_ptr<DropTableStatement> DropTableStatement::parse(TokenStream& ts) {
    std::vector<std::string> tableNames =
      parser::parseCommaSeparated(ts, parser::parseNextIdentifier);
    URSQL_EXPECT(!ts.hasNext(), RedundantInput, ts);
    return std::make_unique<DropTableStatement>(std::move(tableNames));
}

}  // namespace ursql
