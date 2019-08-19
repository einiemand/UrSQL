cmake_minimum_required(VERSION 3.0)
project(UrSQL)

set(URSQL_HEADER
        include/common/Error.hpp
        include/common/Macros.hpp
        include/common/Util.hpp
        include/controller/BasicInterpreter.hpp
        include/controller/DBManager.hpp
        include/controller/Interpreter.hpp
		include/controller/Processor.hpp
        include/controller/SQLInterpreter.hpp
        include/model/Attribute.hpp
        include/model/Block.hpp
        include/model/BufferStream.hpp
        include/model/Database.hpp
        include/model/DBStatement.hpp
        include/model/Entity.hpp
        include/model/Filter.hpp
        include/model/FolderReader.hpp
        include/model/Keyword.hpp
        include/model/Order.hpp
        include/model/Row.hpp
        include/model/SQLStatement.hpp
        include/model/Statement.hpp
        include/model/Storable.hpp
        include/model/Storage.hpp
        include/model/TOC.hpp
        include/model/Token.hpp
        include/model/Tokenizer.hpp
        include/model/Value.hpp
        include/view/View.hpp
        )

set(URSQL_SOURCE
        src/common/Error.cpp
        src/controller/BasicInterpreter.cpp
        src/controller/DBManager.cpp
		src/controller/Processor.cpp
        src/controller/SQLInterpreter.cpp
        src/model/Attribute.cpp
        src/model/Block.cpp
        src/model/BufferStream.cpp
        src/model/Database.cpp
        src/model/DBStatement.cpp
        src/model/Entity.cpp
        src/model/Filter.cpp
        src/model/FolderReader.cpp
        src/model/Keyword.cpp
        src/model/Order.cpp
        src/model/Row.cpp
        src/model/SQLStatement.cpp
        src/model/Statement.cpp
        src/model/Storable.cpp
        src/model/Storage.cpp
        src/model/TOC.cpp
        src/model/Token.cpp
        src/model/Tokenizer.cpp
        src/model/Value.cpp
        src/view/View.cpp
        )

set(URSQL_MAIN ${CMAKE_SOURCE_DIR}/src/main.cpp)
include_directories(${CMAKE_SOURCE_DIR}/include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -pedantic")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	string(APPEND CMAKE_CXX_FLAGS " -O3")
endif()
message(STATUS "Flags: ${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 17)

add_library(ursqlmvc ${URSQL_HEADER} ${URSQL_SOURCE})

add_executable(ursql ${URSQL_MAIN})
target_link_libraries(ursql ursqlmvc)
