file(GLOB_RECURSE URSQL_HEADERS ${CMAKE_SOURCE_DIR}/include/parser/*.hpp ${CMAKE_SOURCE_DIR}/include/exception/*.hpp
        ${CMAKE_SOURCE_DIR}/include/model/*.hpp
        ${CMAKE_SOURCE_DIR}/include/persistence/*.hpp
        ${CMAKE_SOURCE_DIR}/include/statement/*.hpp
        ${CMAKE_SOURCE_DIR}/include/common/*.hpp
        ${CMAKE_SOURCE_DIR}/include/controller/*.hpp
        ${CMAKE_SOURCE_DIR}/include/view/*.hpp
)
file(GLOB_RECURSE URSQL_SOURCES parser/*.cpp exception/*.cpp model/*.cpp persistence/*.cpp statement/*.cpp controller/*.cpp view/*.cpp)

add_library(ursql_lib ${URSQL_HEADERS} ${URSQL_SOURCES})
target_compile_definitions(ursql_lib PUBLIC _GNU_SOURCE)
target_compile_options(ursql_lib PUBLIC -g -Wall -Wextra -pedantic)
target_include_directories(ursql_lib PRIVATE ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})
target_link_libraries(ursql_lib PUBLIC Boost::stacktrace_basic)

add_executable(ursql main.cpp)
target_include_directories(ursql PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ursql ursql_lib readline)
