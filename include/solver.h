#ifndef SOLVER_H
#define SOLVER_H

#include "maze.h"
#include "common.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

// Mutex grid for protecting maze regions
typedef struct {
    pthread_mutex_t *mutexes;   // Array of mutexes
    vec2_t grid_dimensions;     // Dimensions of the mutex grid
    int_t region_size;          // Size of each square region in maze cells
} mutex_grid_t;

// Exploration tracking structure - parallel to the maze structure
typedef struct {
    bool *data;                 // Color
    direction_t *explored_from; // Direction from which each cell was explored (0 if not explored)
    vec2_t dimensions;          
    vec2_t true_dimensions;     
    int_t start;                
    mutex_grid_t mutex_grid;    // Grid of mutexes
} exploration_map_t;

// Macro to access exploration map
#define explored_at(exp_map, _x, _y) \
    ((exp_map).data[(exp_map).start + (_x) + (_y) * (exp_map).true_dimensions.x])

// Get the mutex index for a given position
#define get_mutex_index(exp_map, _x, _y) \
    (((_x) / (exp_map).mutex_grid.region_size) + \
     ((_y) / (exp_map).mutex_grid.region_size) * (exp_map).mutex_grid.grid_dimensions.x)

// Bifurcation (position to explore)
typedef struct {
    vec2_t position;            // Position in the maze
    direction_t came_from;      // Direction from which this position was reached
} bifurcation_t;

// Thread-safe buffer for bifurcations to be explored
typedef struct {
    bifurcation_t *data;        // Array of bifurcations
    int_t capacity;             // Maximum capacity
    int_t head;                 // Index where we read from (FIFO)
    int_t tail;                 // Index where we write to (FIFO)
    int_t count;                // Current number of elements
    pthread_mutex_t mutex;      // Mutex for thread safety and coordination
    pthread_cond_t work_available; // Condition for work availability or termination
} bifurcation_buffer_t;

// Worker position tracking
typedef struct {
    vec2_t position;            // Current position of the worker
    bool is_active;             // Whether worker is actively exploring
} worker_position_t;

// Shared state for all solver threads
typedef struct {
    maze_t maze;                        // The maze to solve
    exploration_map_t explored;         // Which cells have been explored
    bifurcation_buffer_t bifurcations;  // Queue of bifurcations to explore (also handles coordination)
    vec2_t goal;                        // Goal position (bottom-right corner)
    bool solution_found;                // Flag indicating if any thread found the goal
    uint8_t num_workers;                // Number of worker threads
    int active_workers;                 // Number of currently active workers
    bool shutdown;                      // Flag to signal all threads to terminate
    worker_position_t *worker_positions; // Array of worker positions for visualization
    pthread_mutex_t viz_mutex;          // Mutex for visualization updates
    bool enable_visualization;          // Enable real-time visualization
} solver_state_t;

// Arguments passed to each worker thread
typedef struct {
    solver_state_t *state;      // Shared state
    uint8_t worker_id;          // Thread identifier
} worker_args_t;


// ==============================================================================
// UTILITY FUNCTIONS
// ==============================================================================

// Move in a direction
vec2_t move_direction(vec2_t pos, direction_t dir);

// Get opposite direction
direction_t opposite_direction(direction_t dir);


// ==============================================================================
// SOLVER STATE FUNCTIONS
// ==============================================================================

// Initializes the solver state
void init_solver_state(solver_state_t *state, maze_t maze, uint8_t num_workers, bool enable_viz);

// Cleans up solver state
void cleanup_solver_state(solver_state_t *state);


// ==============================================================================
// SOLVER FUNCTIONS
// ==============================================================================

// Main solver function - launches worker threads and solves the maze
void solve_maze(maze_t maze, uint8_t num_workers, bool enable_viz);

#endif // SOLVER_H
