find_package(GTest REQUIRED)

enable_testing()

include(GoogleTest)

file(GLOB_RECURSE URSQL_TEST_HEADERS *.hpp)
add_executable(ursql_test UrSQLTest.cpp ${URSQL_TEST_HEADERS})

target_link_libraries(
        ursql_test
        ursql_lib
        GTest::gtest_main
)

gtest_discover_tests(ursql_test)
