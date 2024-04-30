find_package(GTest REQUIRED)

enable_testing()

include(GoogleTest)

file(GLOB_RECURSE URSQL_TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
add_executable(ursql_test UrSQLTest.cpp ${URSQL_TEST_HEADERS})

target_include_directories(ursql_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(
        ursql_test
        ursql_lib
        GTest::gtest_main
)

gtest_discover_tests(ursql_test)
