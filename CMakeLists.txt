cmake_minimum_required(VERSION 3.17)
project(ConcurrentUtilities VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(ConcurrentQueue)
add_subdirectory(Catch2)