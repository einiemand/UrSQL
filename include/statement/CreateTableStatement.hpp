#include <vector>

#include "SQLStatement.hpp"
#include "model/Attribute.hpp"

namespace ursql {

class CreateTableStatement : public SQLStatement {
public:
    CreateTableStatement(std::string tableName,
                         std::vector<Attribute> attributes);
    ~CreateTableStatement() override = default;

    ExecuteResult run(DBManager& dbManager) const override;

    static std::unique_ptr<CreateTableStatement> parse(TokenStream& ts);

private:
    const std::vector<Attribute> attributes_;
};

}  // namespace ursql
