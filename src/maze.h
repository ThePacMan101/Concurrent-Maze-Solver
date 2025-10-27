#ifndef MAZE_H
#define MAZE_H

#include "common.h"

typedef uint32_t int_t;
typedef uint8_t direction_t;

typedef struct {
    int_t x,y;
} vec2_t;


typedef struct{
    direction_t open_directions; // <0000 WSEN>, so <0000 0110> means South and East are open directions
    // TODO: decide whether the vertex position should be stored here or not
} maze_vertex_t;
enum direction{ NORTH=1,EAST=2,SOUTH=4,WEST=8 };


typedef struct{
    maze_vertex_t * data;
    vec2_t true_dimensions;
    int_t start;
    vec2_t dimensions;
} maze_t;
#define maze_at(maze,_x,_y) ((maze).data[(maze).start + (_x) + (_y)*(maze).true_dimensions.x])


// alocates the *data* for a maze for the first time
// usage:
// maze_t maze;
// alloc_maze(&maze,10,10);
// Now maze.data is not NULL
extern void alloc_maze(maze_t *maze,int_t dim_x, int_t dim_y);


// gets a sub-maze of a bigger maze, that points to the same data in memory
// suppose we have the maze:
// 0x01 0x02 0x03
// 0x04 0x05 0x06
// 0x07 0x08 0x09
// if se use get_sub_maze(maze,1,1,2,2); the result is:
// 0x05 0x06
// 0x08 0x09
// this references the same data, it's not a copy of the original maze
// it therefore, does not allocate more memory
extern maze_t get_sub_maze(maze_t maze,int_t start_x,int_t start_y,int_t end_x,int_t end_y);


// prints maze to shell, keep in mind a large maze may be
// hard to understand when using this visualization.
extern void print_maze(maze_t maze);


// gets a random direction between one of the available directions
extern direction_t random_direction(direction_t available);


// splits a maze into parts, that points to the same data in memory
extern void split_maze(maze_t maze ,maze_t *target,uint8_t parts);


// generates a random maze using the Markov Chain Montecarlo method
extern maze_t generate_random_maze_MCMC(int_t dim_x,int_t dim_y, uint64_t number_of_iterations);


// generates a random maze using the Markov Chain Montecarlo method in parallel
// the maze must be square, its side must be a power of two, and the number of workers
// must also be a power of two.
extern maze_t generate_random_maze_MCMC_parallel(int_t dim_x,int_t dim_y,uint64_t number_of_iterations,uint8_t workers);


extern void test_hilbert();




#endif