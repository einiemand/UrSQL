#include "statement/CreateTableStatement.hpp"

#include <unordered_set>

#include "controller/DBManager.hpp"
#include "exception/UserError.hpp"
#include "model/Database.hpp"
#include "parser/Parser.hpp"
#include "parser/TokenStream.hpp"
#include "view/RowsAffectedTextView.hpp"

namespace ursql {

CreateTableStatement::CreateTableStatement(std::string tableName,
                                           std::vector<Attribute> attributes)
    : SingleTableStatement(std::move(tableName)),
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
    URSQL_EXPECT(!attributes_.empty(), InvalidCommand,
                 "attributes can't be empty");
    bool autoIncDefined = false;
    bool primaryKeyDefined = false;
    std::unordered_set<std::string> attrNames;
    for (auto& attribute : attributes_) {
        if (attribute.isAutoInc()) {
            URSQL_EXPECT(!autoIncDefined, InvalidCommand,
                         "there can be only one auto_increment column");
            autoIncDefined = true;
            URSQL_EXPECT(attribute.isPrimary(), InvalidCommand,
                         "auto_increment column should be a key");
            URSQL_EXPECT(attribute.getType() == ValueType::int_type, MisMatch,
                         "auto_increment column should be integer type");
        }
        if (attribute.isPrimary()) {
            URSQL_EXPECT(!primaryKeyDefined, InvalidCommand,
                         "multiple primary keys defined");
            primaryKeyDefined = true;
        }
        URSQL_EXPECT(
          attribute.getDefaultValue().castableTo(attribute.getType()), MisMatch,
          std::format("default value type doesn't match '{}' type",
                      attribute.getName()));
        URSQL_EXPECT(
          attrNames.insert(attribute.getName()).second, InvalidCommand,
          std::format("duplicate column name '{}'", attribute.getName()));
    }
}

}  // namespace ursql
