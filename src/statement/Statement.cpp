#include "statement/Statement.hpp"

#include <format>

#include "view/View.hpp"
#include <memory>

namespace ursql {

ExecuteResult NopStatement::run(DBManager&) const {
    return { nullptr, false };
}

ExecuteResult HelpStatement::run(DBManager&) const {
    constexpr const char str[] =
      "help    - show guide on how to use UrSQL\n"
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

}  // namespace ursql
