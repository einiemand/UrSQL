project (UrSQL)

file (GLOB VIEW_SOURCES *.cpp *.hpp)
set(CMAKE_CXX_STANDARD 17)

add_library (view ${VIEW_SOURCES})
target_link_libraries (view model controller)
