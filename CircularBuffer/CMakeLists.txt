project (CircularBuffer)

add_library (CircularBuffer INTERFACE)

target_include_directories (CircularBuffer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(CircularBuffer INTERFACE Threads::Threads)

add_subdirectory (Tests)
