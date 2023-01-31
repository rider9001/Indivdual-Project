cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(CamRunTest)
add_executable(CamRunTest main.cpp)
include_directories(CamRunTest src/)
set(CMAKE_CXX_FLAGS "-Wall -pthread -O3")
