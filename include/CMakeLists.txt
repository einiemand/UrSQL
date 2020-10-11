add_subdirectory(common)
add_subdirectory(controller)
add_subdirectory(model)
add_subdirectory(view)

set(URSQL_HEADER
        ${URSQL_HEADER_COMMON}
        ${URSQL_HEAEDER_CONTROLLER}
        ${URSQL_HEADER_MODEL}
        ${URSQL_HEADER_VIEW}
        )
