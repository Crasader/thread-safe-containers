add_executable (TestConcurrentQueue TestConcurrentQueue.cpp
                                    SequentialTests.cpp
                                    ConcurrentTests.cpp)

target_link_libraries (TestConcurrentQueue PRIVATE Catch2)        # link to the testing library
target_link_libraries (TestConcurrentQueue PRIVATE ConcurrentQueue)  # link to the business logic to be tested

# Add test to CTest. Define the command used to run the test (catch flags)
# Catch2 flag info: https://github.com/catchorg/Catch2/blob/master/docs/command-line.md
add_test(NAME TestConcurrentQueue 
            COMMAND TestConcurrentQueue --reporter junit --name DataStructures --filenames-as-tags -s --out ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestConcurrentQueue_results.xml)

# Add CTest label. Tests with the same label are grouped together. 
set_tests_properties (TestConcurrentQueue PROPERTIES LABELS DataStructures) 

set_target_properties (TestConcurrentQueue PROPERTIES FOLDER UnitTests/DataStructures) 
    
