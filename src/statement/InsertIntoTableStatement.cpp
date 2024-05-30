#include "statement/InsertIntoTableStatement.hpp"

#include "controller/DBManager.hpp"
#include "exception/UserError.hpp"
#include "model/Database.hpp"
#include "parser/Parser.hpp"
#include "parser/TokenStream.hpp"
#include "view/RowsAffectedTextView.hpp"

namespace ursql {

InsertIntoTableStatement::InsertIntoTableStatement(
  std::string tableName, std::optional<std::vector<std::string>> attrNames,
  std::vector<std::vector<Value>> valueLists)
    : SingleTableStatement(std::move(tableName)),
      attrNames_(std::move(attrNames)),
      valueLists_(std::move(valueLists)) {}

ExecuteResult InsertIntoTableStatement::run(DBManager& dbManager) const {
    Database* activeDB = dbManager.getActiveDB();
    URSQL_EXPECT(activeDB, NoActiveDB, );
    activeDB->insertIntoTable(tableName_, attrNames_, valueLists_);
    return { std::make_unique<RowsAffectedTextView>(valueLists_.size()),
             false };
}

std::unique_ptr<InsertIntoTableStatement> InsertIntoTableStatement::parse(
  TokenStream& ts) {
    URSQL_EXPECT(ts.skipIf(Keyword::into_kw), MissingInput, "'into'");
    std::string tableName = parser::parseNextIdentifier(ts);
    std::optional<std::vector<std::string>> attrNames;
    if (ts.skipIf(Punctuation::lparen)) {
        attrNames =
          parser::parseCommaSeparated(ts, parser::parseNextIdentifier);
        URSQL_EXPECT(ts.skipIf(Punctuation::rparen), MissingInput,
                     "')' after column names");
    }
    URSQL_EXPECT(ts.skipIf(Keyword::values_kw), MissingInput, "'values'");
    std::vector<std::vector<Value>> valueRows =
      parser::parseCommaSeparated(ts, [](TokenStream& ts1) {
          URSQL_EXPECT(ts1.skipIf(Punctuation::lparen), MissingInput,
                       "'(' before value list");
          std::vector<Value> valueRow =
            parser::parseCommaSeparated(ts1, Value::parse);
          URSQL_EXPECT(ts1.skipIf(Punctuation::rparen), MissingInput,
                       "')' after value list");
          return valueRow;
      });
    URSQL_EXPECT(!ts.hasNext(), RedundantInput, ts);
    return std::make_unique<InsertIntoTableStatement>(
      std::move(tableName), std::move(attrNames), std::move(valueRows));
}

}  // namespace ursql
