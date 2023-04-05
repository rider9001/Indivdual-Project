cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(velocityProc)
add_executable(velocityProc main.cpp)
include_directories(velocityProc src/)
set(CMAKE_CXX_FLAGS "-Wall -O3 -pthread -std=c++20 -static")
