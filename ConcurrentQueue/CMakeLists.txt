add_library (ConcurrentQueue INTERFACE)

target_include_directories (ConcurrentQueue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory (Tests)
