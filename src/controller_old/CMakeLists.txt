set(URSQL_SOURCE_CONTROLLER
        BasicInterpreter.cpp
        DBManager.cpp
        Processor.cpp
        SQLInterpreter.cpp
        )

add_library(ursql_controller ${URSQL_HEADER_CONTROLLER} ${URSQL_SOURCE_CONTROLLER})
