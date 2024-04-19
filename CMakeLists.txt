cmake_minimum_required(VERSION 3.0...3.28)
project(UrSQL)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -pedantic")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	string(APPEND CMAKE_CXX_FLAGS " -O3")
endif()
message(STATUS "Flags: ${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(test)
