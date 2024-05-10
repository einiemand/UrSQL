#include "common/Message.hpp"
#include "parser/StatementParser.hpp"
#include "parser/TokenStream.hpp"
#include "statement/Statement.hpp"
#include "exception/UserError.hpp"
#include "exception/InternalError.hpp"
#include <iostream>

#include <readline/readline.h>
#include <sstream>

namespace ursql {

std::ostream& out = std::cout;

}

int main(int argc, char* argv[]) {
    using namespace ursql;

    std::stringstream strStream;
    while (true) {
        try {
            char* line = readline("ursql> ");

            TokenStream tokenStream(iss);
            auto pStmt = parser::parse(tokenStream);
            pStmt->validate();
            if (!pStmt->execute()) {
                break;
            }
        } catch (const UserError& userError) {
            out << userError.what();
        } catch (const FatalError& fatalError) {
            out << fatalError.what();
            break;
        } catch (const InternalError& internalError) {
            out << internalError.what();
        }
    }

    return 0;
}
