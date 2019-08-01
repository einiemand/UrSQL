cmake_minimum_required(VERSION 3.0)
project(UrSQL)

set(SRC_HEADER
        Error.hpp
        Keyword.hpp
        Util.hpp
        model/Attribute.hpp
        model/Block.hpp
        model/BufferStream.hpp
        model/Database.hpp
        model/DBStatement.hpp
        model/Entity.hpp
        model/Filter.hpp
        model/FolderReader.hpp
        model/Order.hpp
        model/Row.hpp
        model/SQLStatement.hpp
        model/Statement.hpp
        model/Storable.hpp
        model/Storage.hpp
        model/TOC.hpp
        model/Token.hpp
        model/Tokenizer.hpp
        model/Value.hpp
        view/View.hpp
        controller/BasicProcessor.hpp
        controller/DBManager.hpp
        controller/Interpreter.hpp
        controller/SQLInterpreter.hpp
        )

set(SRC_SOURCE
        Error.cpp
        Keyword.cpp
        model/Attribute.cpp
        model/Block.cpp
        model/BufferStream.cpp
        model/Database.cpp
        model/DBStatement.cpp
        model/Entity.cpp
        model/Filter.cpp
        model/FolderReader.cpp
        model/Order.cpp
        model/Row.cpp
        model/SQLStatement.cpp
        model/Statement.cpp
        model/Storable.cpp
        model/Storage.cpp
        model/TOC.cpp
        model/Token.cpp
        model/Tokenizer.cpp
        model/Value.cpp
        view/View.cpp
        controller/BasicProcessor.cpp
        controller/DBManager.cpp
        controller/SQLInterpreter.cpp
        )

set(MAIN main.cpp)

include_directories(
        .
        model
        view
        controller
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -pedantic -O3")

add_library(ursql ${SRC_HEADER} ${SRC_SOURCE})

add_executable(UrSQL ${MAIN})
target_link_libraries(UrSQL ursql)
