#include "statement/SQLStatement.hpp"

namespace ursql {

SQLStatement::SQLStatement(std::string tableName) : tableName_(std::move(tableName)) {}

}
