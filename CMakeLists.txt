cmake_minimum_required(VERSION 3.28)
project(UrSQL)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.65 REQUIRED COMPONENTS stacktrace_basic)

add_subdirectory(src)
add_subdirectory(test)
