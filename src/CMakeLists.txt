cmake_minimum_required (VERSION 3.0)

project (UrSQL)

add_subdirectory (controller)
add_subdirectory (model)
add_subdirectory (view)

file (GLOB TOP_SOURCES ./*.cpp ./*.hpp)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)
set(CMAKE_CXX_STANDARD 17)

add_executable (UrSQL ${TOP_SOURCES})
target_link_libraries (UrSQL model controller view)
