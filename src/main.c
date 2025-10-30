#include "solver.h"
#include "maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>

int main(int argc, char **argv) {
    // Set locale for wide character support
    setlocale(LC_ALL, "");
    
    srand(time(NULL));
    
    // maze size
    int_t maze_side_size = 16;
    if (argc > 1) {
        maze_side_size = atoi(argv[1]);
        if (maze_side_size < 4) maze_side_size = 4;
    }
    
    uint8_t num_workers = (argc > 2) ? atoi(argv[2]) : 4;
    if (num_workers < 1) num_workers = 1;
    if (num_workers > 64) num_workers = 64;
    
    bool enable_iterative_visualization = true;
    if (argc > 3) {
        enable_iterative_visualization = (atoi(argv[3]) != 0);
    }
    
    // Generate maze
    wprintf(L"Generating %dx%d maze...\n", maze_side_size, maze_side_size);
    uint64_t iterations = maze_side_size * maze_side_size * maze_side_size;
    maze_t maze = generate_random_maze_MCMC(maze_side_size, maze_side_size, iterations);
    wprintf(L"Maze generated!\n\n");
    
    // Solve the maze
    solve_maze(maze, num_workers, enable_iterative_visualization);
    
    // Cleanup
    free(maze.data);
    
    return 0;
}
