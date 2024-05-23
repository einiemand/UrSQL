#include "statement/DBStatement.hpp"

#include <format>

#include "controller/DBManager.hpp"
#include "exception/InternalError.hpp"
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

class DescDBView : public TabularView {
public:
    explicit DescDBView(const std::vector<BlockType>& blockTypes)
        : TabularView({ "Index", "Type" }, _blockTypes2Rows(blockTypes)) {}

    ~DescDBView() override = default;

private:
    static std::vector<std::vector<Value>> _blockTypes2Rows(
      const std::vector<BlockType>& blockTypes) {
        std::vector<std::vector<Value>> valueRows;
        valueRows.reserve(blockTypes.size());
        for (std::size_t i = 0; i < blockTypes.size(); ++i) {
            std::vector<Value> valueRow;
            valueRow.emplace_back(static_cast<Value::int_t>(i));
            valueRow.emplace_back(_blockType2String(blockTypes[i]));
            valueRows.push_back(std::move(valueRow));
        }
        return valueRows;
    }

    static std::string _blockType2String(BlockType blockType) {
        switch (blockType) {
        case BlockType::toc:
            return "TOC";
        case BlockType::entity:
            return "Entity";
        case BlockType::index:
            return "Index";
        case BlockType::row:
            return "Row";
        case BlockType::free:
            return "Free";
        default:
            URSQL_UNREACHABLE(std::format("unknown block type {}", blockType));
        }
    }
};

DescDBStatement::DescDBStatement(std::string dbName)
    : DBStatement(std::move(dbName)) {}

ExecuteResult DescDBStatement::run(DBManager& dbManager) const {
    std::vector<BlockType> blockTypes;
    if (Database* activeDB = dbManager.getActiveDB();
        activeDB && activeDB->getName() == dbName_)
    {
        blockTypes = activeDB->getBlockTypes();
    } else {
        std::unique_ptr<Database> database = DBManager::getDBByName(dbName_);
        blockTypes = database->getBlockTypes();
    }
    return { std::make_unique<DescDBView>(blockTypes), false };
}

std::unique_ptr<DescDBStatement> DescDBStatement::parse(TokenStream& ts) {
    return std::make_unique<DescDBStatement>(
      parser::parseNextIdentifierAsLast(ts));
}

}  // namespace ursql
