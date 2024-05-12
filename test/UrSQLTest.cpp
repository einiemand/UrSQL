#include "model/ValueTest.hpp"
#include "parser/TokenStreamTest.hpp"
#include "parser/TokenTest.hpp"

namespace ursql {

std::ostream& out = std::cout;
std::ostream& err = std::cerr;

}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}