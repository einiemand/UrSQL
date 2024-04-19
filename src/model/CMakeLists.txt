file(GLOB URSQL_SOURCE_MODEL *.cpp)

add_library(ursql_model ${URSQL_HEADER_MODEL} ${URSQL_SOURCE_MODEL})
