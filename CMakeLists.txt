cmake_minimum_required(VERSION 3.7)
project(aho_corasick)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(aho_corasick ${SOURCE_FILES})