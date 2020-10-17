#include "controller/BasicInterpreter.hpp"
#include "controller/DBManager.hpp"
#include "controller/Processor.hpp"
#include "controller/SQLInterpreter.hpp"
#include "model/Database.hpp"

int main(int argc, char* argv[]) {
    using namespace UrSQL;
    if (argc > 2) {
        defaultOutput << "Too many arguments(>2) to unpack\n";
        return 0;
    }

    Processor theProcessor;
    theProcessor.addInterpreter<DBManager>();
    theProcessor.addInterpreter<BasicInterpreter>();
    theProcessor.addInterpreter<SQLInterpreter>();
    if (argc == 2) {
        std::ifstream theFileStream;
        theFileStream.open(argv[1]);
        if (theFileStream.is_open()) {
            theProcessor.consume(
              theFileStream,
              [&theProcessor](const std::string& aCommandString) {
                  defaultOutput << aCommandString << ";\n";
                  return theProcessor.consumeOne(aCommandString);
              });
        }
        else {
            defaultOutput << "Cannot open '" << argv[1] << "'\n";
        }
        return 0;
    }

    theProcessor.consume(std::cin,
                         [&theProcessor](const std::string& aCommandString) {
                             return theProcessor.consumeOne(aCommandString);
                         });

    return 0;
}
