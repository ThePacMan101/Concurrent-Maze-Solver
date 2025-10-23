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

// generates a random maze using the Markov Chain Montecarlo method
extern maze_t generate_random_maze(int_t dim_x,int_t dim_y, uint64_t number_of_iterations);

// generates a random maze using the Markov Chain Montecarlo method in parallel
extern maze_t generate_random_maze_parallel(int_t dim_x,int_t dim_y,uint64_t number_of_iterations,uint8_t workers);

// prints maze to shell, keep in mind a large maze may be
// hard to understand when using this visualization.
extern void print_maze(maze_t maze);

extern void split_maze(maze_t maze ,maze_t *target,uint8_t parts);

#endif