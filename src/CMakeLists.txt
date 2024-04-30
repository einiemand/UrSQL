file(GLOB_RECURSE URSQL_HEADERS ${CMAKE_SOURCE_DIR}/include/parser/*.hpp ${CMAKE_SOURCE_DIR}/include/exception/*.hpp)
file(GLOB_RECURSE URSQL_SOURCES parser/*.cpp exception/*.cpp)

find_package(Boost 1.65 REQUIRED COMPONENTS stacktrace_basic)

#list(REMOVE_ITEM URSQL_SOURCES main.cpp)

add_library(ursql_lib ${URSQL_HEADERS} ${URSQL_SOURCES})
target_compile_definitions(ursql_lib PUBLIC _GNU_SOURCE)
target_compile_options(ursql_lib PUBLIC -g -Wall -Wextra -pedantic)
target_include_directories(ursql_lib PRIVATE ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})
target_link_libraries(ursql_lib PRIVATE Boost::stacktrace_basic)

#add_executable(ursql main.cpp)
#target_link_libraries(ursql ursql_lib)
