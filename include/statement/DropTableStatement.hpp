#pragma once

#include "TableStatement.hpp"

namespace ursql {

class DropTableStatement : public MultiTableStatement {
public:
    explicit DropTableStatement(std::vector<std::string> tableNames);
    ~DropTableStatement() override = default;

    [[nodiscard]] ExecuteResult run(DBManager& dbManager) const override;

    static std::unique_ptr<DropTableStatement> parse(TokenStream& ts);
};

}  // namespace ursql
