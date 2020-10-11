add_subdirectory(common)
add_subdirectory(controller)
add_subdirectory(model)
add_subdirectory(view)

set(URSQL_MVC
        ursql_common
        ursql_controller
        ursql_model
        ursql_view
        )
set(URSQL_MAIN main.cpp)

add_executable(ursql ${URSQL_MAIN})
target_link_libraries(ursql ${URSQL_MVC})
