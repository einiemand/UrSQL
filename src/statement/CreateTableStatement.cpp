#include "statement/CreateTableStatement.hpp"

#include "controller/DBManager.hpp"
#include "exception/UserError.hpp"
#include "model/Database.hpp"
#include "parser/Parser.hpp"
#include "parser/TokenStream.hpp"
#include "view/RowsAffectedTextView.hpp"
#include <unordered_set>

namespace ursql {

CreateTableStatement::CreateTableStatement(std::string tableName,
                                           std::vector<Attribute> attributes)
    : SQLStatement(std::move(tableName)),
      attributes_(std::move(attributes)) {}

ExecuteResult CreateTableStatement::run(DBManager& dbManager) const {
    Database* activeDB = dbManager.getActiveDB();
    URSQL_EXPECT(activeDB, NoActiveDB, );
    _validateAttributes();
    activeDB->createTable(tableName_, attributes_);
    return { std::make_unique<RowsAffectedTextView>(0), false };
}

std::unique_ptr<CreateTableStatement> CreateTableStatement::parse(
  TokenStream& ts) {
    std::string tableName = parser::parseNextIdentifier(ts);
    URSQL_EXPECT(ts.skipIf(Punctuation::lparen), MissingInput,
                 "'(' after table name");
    std::vector<Attribute> attributes;
    do {
        attributes.push_back(Attribute::parse(ts));
    } while (ts.skipIf(Punctuation::comma));
    URSQL_EXPECT(ts.skipIf(Punctuation::rparen), MissingInput,
                 "')' after attribute list");
    return std::make_unique<CreateTableStatement>(std::move(tableName),
                                                  std::move(attributes));
}

void CreateTableStatement::_validateAttributes() const {
    URSQL_EXPECT(!attributes_.empty(), InvalidCommand, "attributes can't be empty");
    std::size_t autoIncCnt = 0;
    std::unordered_set<std::string> attrNames;
    for (auto& attribute : attributes_) {
        if (attribute.isAutoInc()) {
            ++autoIncCnt;
            URSQL_EXPECT(autoIncCnt <= 1, InvalidCommand, "there can be only one auto inc column");
        }
        URSQL_EXPECT(attrNames.insert(attribute.getName()).second,
                     InvalidCommand, std::format("duplicate column name '{}'", attribute.getName()));
    }
}

}  // namespace ursql
