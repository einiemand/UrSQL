set(URSQL_SOURCE_MODEL
        Attribute.cpp
        Block.cpp
        BufferStream.cpp
        Database.cpp
        DBStatement.cpp
        Entity.cpp
        Filter.cpp
        FolderReader.cpp
        Keyword.cpp
        Order.cpp
        Row.cpp
        SQLStatement.cpp
        Statement.cpp
        Storable.cpp
        Storage.cpp
        TOC.cpp
        Token.cpp
        Tokenizer.cpp
        Value.cpp
        )

add_library(ursql_model ${URSQL_HEADER_MODEL} ${URSQL_SOURCE_MODEL})
