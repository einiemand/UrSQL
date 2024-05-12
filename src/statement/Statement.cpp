#include "statement/Statement.hpp"

#include <format>
#include <iostream>

#include "common/Messaging.hpp"
#include "exception/InternalError.hpp"
#include "parser/TokenEnums.hpp"

namespace ursql {

void NopStatement::validate() const {}

bool NopStatement::execute() const {
    return true;
}

BasicStatement::BasicStatement(Keyword keyword) : Statement(), keyword_(keyword) {}


void BasicStatement::validate() const {
}

bool BasicStatement::execute() const {
    switch (keyword_) {
    case Keyword::help_kw:
        out << "help    - show guide on how to use UrSQL" << '\n'
            << "version - show UrSQL version" << '\n'
            << "quit    - quit UrSQL\n";
        return true;
    case Keyword::version_kw:
        out << "UrSQL 1.0\n";
        return true;
    case Keyword::quit_kw:
        out << "Bye\n";
        return false;
    default:
        URSQL_UNREACHABLE(std::format("unsupported keyword {} for a basic statement", keyword_));
    }
}

}  // namespace ursql
