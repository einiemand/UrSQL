file(GLOB_RECURSE URSQL_HEADERS ${CMAKE_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE URSQL_SOURCES *.cpp)

list(REMOVE_ITEM URSQL_SOURCES main.cpp)

add_library(ursql_lib ${URSQL_SOURCES})
target_include_directories(ursql_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(ursql main.cpp)
target_link_libraries(ursql ursql_lib)
