project (UrSQL)

file (GLOB MODEL_SOURCES *.cpp *.hpp)
set(CMAKE_CXX_STANDARD 17)

add_library (model ${MODEL_SOURCES})
target_link_libraries (model view controller)
