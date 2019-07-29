project (UrSQL)

file (GLOB CONTROLLER_SOURCES *.cpp *.hpp)
set(CMAKE_CXX_STANDARD 17)

add_library (controller ${CONTROLLER_SOURCES})
target_link_libraries (controller model view)
