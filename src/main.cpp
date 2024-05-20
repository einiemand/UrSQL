#include <readline/readline.h>

#include <iostream>
#include <sstream>

#include "common/Finally.hpp"
#include "controller/DBManager.hpp"
#include "exception/InternalError.hpp"
#include "parser/Parser.hpp"
#include "parser/SQLBlob.hpp"
#include "parser/TokenStream.hpp"
#include "statement/Statement.hpp"

namespace ursql {

std::ostream& out = std::cout;
std::ostream& err = std::cerr;

}  // namespace ursql

int main(int argc, char* argv[]) {
    using namespace ursql;

    DBManager dbManager;

    SQLBlob blob;
    bool quit = false;
    do {
        char* line = readline("ursql> ");
        if (!line) {
            continue;
        }
        Finally cleanup([line]() {
            std::free(line);
        });
        std::istringstream input(line);
        while (!quit && input >> blob) {
            try {
                try {
                    if (blob.ready()) {
                        TokenStream tokenStream = blob.tokenize();
                        auto pStmt = parser::parse(tokenStream);
                        ExecuteResult result = pStmt->run(dbManager);
                        result.showView(out);
                        quit = result.quit();
                    }
                } catch (const FatalError& fatalError) {
                    quit = true;
                    throw;
                } catch (const std::exception&) {
                    throw;
                }
            } catch (const std::exception& e) {
                err << e.what() << '\n';
                const boost::stacktrace::stacktrace* st =
                  boost::get_error_info<Traced>(e);
                if (st) {
                    err << *st << '\n';
                }
                err << '\n';
            }
        }
    } while (!quit);

    return 0;
}
