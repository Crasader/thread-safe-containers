project (ConcurrentQueue)

add_library (${PROJECT_NAME} INTERFACE)

target_include_directories (${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (${COMMON_TESTS})
    add_subdirectory (Tests)
endif(${COMMON_TESTS})