#include "parser/TokenIteratorTest.hpp"
#include "parser/TokenTest.hpp"

int main(int argc, char** argv) {
    std::cerr.rdbuf(nullptr);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}