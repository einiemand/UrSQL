cmake_minimum_required(VERSION 3.0...3.28)
project(UrSQL)

set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -pedantic")
message(STATUS "Flags: ${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(test)
