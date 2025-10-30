#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "solver.h"
#include <wchar.h>

// Update worker visualization state (marks worker as active at given position)
void mark_worker_active_at_position(solver_state_t *state, uint8_t worker_id, vec2_t position);

// Mark worker as inactive in visualization
void mark_worker_inactive(solver_state_t *state, uint8_t worker_id);

// Update worker position (keeping active state)
void update_worker_position(solver_state_t *state, uint8_t worker_id, vec2_t position);

// Print maze with exploration information (shows which cells were explored)
void print_maze_explored(solver_state_t *state);

// Print maze with solution path highlighted (traces path from start to goal)
void print_maze_with_solution(solver_state_t *state);

// Print maze with live worker positions (for animation)
void print_maze_animated(solver_state_t *state);

// Visualization thread function (runs continuously during solving)
void* visualizer_thread(void *args);

#endif // VISUALIZATION_H
