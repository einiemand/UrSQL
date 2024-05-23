#include "statement/Statement.hpp"

#include <format>
#include <memory>

#include "controller/DBManager.hpp"
#include "exception/UserError.hpp"
#include "parser/TokenStream.hpp"
#include "view/TabularView.hpp"
#include "view/TextView.hpp"

namespace ursql {

ExecuteResult NopStatement::run(DBManager&) const {
    return { nullptr, false };
}

ExecuteResult HelpStatement::run(DBManager&) const {
    constexpr const char str[] = "help    - show guide on how to use UrSQL\n"
                                 "version - show UrSQL version\n"
                                 "quit    - quit UrSQL";
    return { std::make_unique<TextView>(str), false };
}

ExecuteResult VersionStatement::run(DBManager&) const {
    constexpr const char str[] = "UrSQL 1.0";
    return { std::make_unique<TextView>(str), false };
}

ExecuteResult QuitStatement::run(DBManager&) const {
    constexpr const char str[] = "Bye";
    return { std::make_unique<TextView>(str), true };
}

class ShowDBView : public TabularView {
public:
    explicit ShowDBView(const std::vector<std::string>& dbNames)
        : TabularView({ "Database" }, _dbNames2Rows(dbNames)) {}

    ~ShowDBView() override = default;

private:
    static std::vector<std::vector<Value>> _dbNames2Rows(
      const std::vector<std::string>& dbNames) {
        std::vector<std::vector<Value>> rows;
        rows.reserve(dbNames.size());
        for (auto& dbName : dbNames) {
            rows.push_back({ Value(dbName) });
        }
        return rows;
    }
};

ExecuteResult ShowDBStatement::run(DBManager&) const {
    std::vector<std::string> dbNames = DBManager::getDatabaseNames();
    return { std::make_unique<ShowDBView>(dbNames), false };
}

std::unique_ptr<ShowDBStatement> ShowDBStatement::parse(TokenStream& ts) {
    URSQL_EXPECT(!ts.hasNext(), RedundantInput, ts);
    return std::make_unique<ShowDBStatement>();
}

}  // namespace ursql
