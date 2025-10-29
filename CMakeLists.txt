cmake_minimum_required(VERSION 3.10)
project(ConcurrentMazeSolver C)

# Set C standard and compiler flags
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3")

# Add the executable target with its source files
add_executable(tests
    src/maze.c
    src/maze_mcmc.c
    src/maze_hilbert.c
    src/special_characters.c
    src/solver.c
    src/tests.c
)

# Add include directory
target_include_directories(tests PUBLIC include)

# Find and link the math and threads libraries
find_package(Threads REQUIRED)
target_link_libraries(tests PRIVATE m Threads::Threads)



# Optional: Add a custom target to clean the project, similar to 'make clean'
add_custom_target(custom_clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove_file ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove_file ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove_file ${CMAKE_BINARY_DIR}/Makefile
    COMMENT "Cleaning build directory..."
)