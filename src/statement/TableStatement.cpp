#include "statement/TableStatement.hpp"

namespace ursql {

SingleTableStatement::SingleTableStatement(std::string tableName)
    : tableName_(std::move(tableName)) {}

MultiTableStatement::MultiTableStatement(std::vector<std::string> tableNames)
    : tableNames_(std::move(tableNames)) {}

}  // namespace ursql
