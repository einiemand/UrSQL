#pragma once

#include <optional>

#include "TableStatement.hpp"
#include "model/Value.hpp"

namespace ursql {

class InsertIntoTableStatement : public SingleTableStatement {
public:
    InsertIntoTableStatement(std::string tableName,
                             std::optional<std::vector<std::string>> attrNames,
                             std::vector<std::vector<Value>> valueLists);
    ~InsertIntoTableStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;

    static std::unique_ptr<InsertIntoTableStatement> parse(TokenStream& ts);

private:
    const std::optional<std::vector<std::string>> attrNames_;
    const std::vector<std::vector<Value>> valueLists_;
};

}  // namespace ursql
