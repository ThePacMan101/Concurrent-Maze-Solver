#include "solver.h"
#include "visualization.h"
#include <unistd.h>

// Region size for mutex grid (each region is REGION_SIZE x REGION_SIZE cells)
#define REGION_SIZE 2

vec2_t move_direction(vec2_t pos, direction_t dir) {
  vec2_t new_pos = pos;
  switch (dir) {
  case NORTH:
    new_pos.y--;
    break;
  case SOUTH:
    new_pos.y++;
    break;
  case EAST:
    new_pos.x++;
    break;
  case WEST:
    new_pos.x--;
    break;
  default:
    break;
  }
  return new_pos;
}

// Get opposite direction
direction_t opposite_direction(direction_t dir) {
  switch (dir) {
  case NORTH:
    return SOUTH;
  case SOUTH:
    return NORTH;
  case EAST:
    return WEST;
  case WEST:
    return EAST;
  default:
    return 0;
  }
}

// Calculates mutex grid dimensions based on maze size
static vec2_t calculate_mutex_grid_dimensions(vec2_t maze_dimensions) {
  // Divide maze into regions (each region is REGION_SIZE x REGION_SIZE cells)

  int_t grid_x =
      (maze_dimensions.x + REGION_SIZE - 1) / REGION_SIZE; // Ceiling division
  int_t grid_y = (maze_dimensions.y + REGION_SIZE - 1) / REGION_SIZE;

  // Ensure at least 1x1
  if (grid_x < 1)
    grid_x = 1;
  if (grid_y < 1)
    grid_y = 1;

  return (vec2_t){grid_x, grid_y};
}

// Allocates an exploration map matching the maze dimensions
static void alloc_exploration_map(exploration_map_t *exp_map, maze_t maze) {
  exp_map->dimensions = maze.dimensions;
  exp_map->true_dimensions = maze.true_dimensions;
  exp_map->start = maze.start;

  // Allocate and initialize to false (unexplored)
  size_t size = maze.true_dimensions.x * maze.true_dimensions.y;
  exp_map->data = (bool *)calloc(size, sizeof(bool));

  if (!exp_map->data) {
    PERROR("Couldn't allocate exploration map with size: %d x %d",
           maze.dimensions.x, maze.dimensions.y);
  }

  // Allocate exploration tracking array
  exp_map->explored_from = (direction_t *)calloc(size, sizeof(direction_t));

  if (!exp_map->explored_from) {
    PERROR("Couldn't allocate exploration tracking array");
  }

  // Initialize mutex grid
  exp_map->mutex_grid.grid_dimensions =
      calculate_mutex_grid_dimensions(maze.dimensions);
  exp_map->mutex_grid.region_size = REGION_SIZE;

  size_t num_mutexes = exp_map->mutex_grid.grid_dimensions.x *
                       exp_map->mutex_grid.grid_dimensions.y;
  exp_map->mutex_grid.mutexes =
      (pthread_mutex_t *)malloc(num_mutexes * sizeof(pthread_mutex_t));

  if (!exp_map->mutex_grid.mutexes) {
    PERROR("Couldn't allocate mutex grid with size: %d x %d",
           exp_map->mutex_grid.grid_dimensions.x,
           exp_map->mutex_grid.grid_dimensions.y);
  }

  // Initialize all mutexes
  for (size_t i = 0; i < num_mutexes; i++) {
    pthread_mutex_init(&exp_map->mutex_grid.mutexes[i], NULL);
  }
}

// Frees the exploration map
static void free_exploration_map(exploration_map_t *exp_map) {
  free(exp_map->data);
  free(exp_map->explored_from);

  size_t num_mutexes = exp_map->mutex_grid.grid_dimensions.x *
                       exp_map->mutex_grid.grid_dimensions.y;

  // Destroy all mutexes
  for (size_t i = 0; i < num_mutexes; i++) {
    pthread_mutex_destroy(&exp_map->mutex_grid.mutexes[i]);
  }

  free(exp_map->mutex_grid.mutexes);
}

static void init_bifurcation_buffer(bifurcation_buffer_t *buffer,
                                    int_t capacity) {
  buffer->capacity = capacity;
  buffer->head = 0;
  buffer->tail = 0;
  buffer->count = 0;

  buffer->data = (bifurcation_t *)malloc(capacity * sizeof(bifurcation_t));
  if (!buffer->data) {
    PERROR("Couldn't allocate bifurcation buffer with capacity: %d", capacity);
  }

  pthread_mutex_init(&buffer->mutex, NULL);
  pthread_cond_init(&buffer->work_available, NULL);
}

static void free_bifurcation_buffer(bifurcation_buffer_t *buffer) {
  pthread_mutex_destroy(&buffer->mutex);
  pthread_cond_destroy(&buffer->work_available);
  free(buffer->data);
}

static void increment_active_workers(solver_state_t *state) {
  pthread_mutex_lock(&state->bifurcations.mutex);
  state->active_workers++;
  pthread_mutex_unlock(&state->bifurcations.mutex);
}

static void decrement_active_workers(solver_state_t *state) {
  pthread_mutex_lock(&state->bifurcations.mutex);
  state->active_workers--;
  pthread_mutex_unlock(&state->bifurcations.mutex);
}

static void decrement_active_workers_and_signal(solver_state_t *state) {
  pthread_mutex_lock(&state->bifurcations.mutex);
  state->active_workers--;
  pthread_cond_broadcast(&state->bifurcations.work_available);
  pthread_mutex_unlock(&state->bifurcations.mutex);
}

void init_solver_state(solver_state_t *state, maze_t maze, uint8_t num_workers,
                       bool enable_iterative_visualization) {
  state->maze = maze;
  state->num_workers = num_workers;
  state->goal = (vec2_t){maze.dimensions.x - 1,
                         maze.dimensions.y - 1}; // hardcoded solution
  state->solution_found = false;
  state->shutdown = false;
  state->active_workers = 0;
  state->enable_visualization = enable_iterative_visualization;

  // Allocate exploration map (includes mutex grid initialization)
  alloc_exploration_map(&state->explored, maze);

  // Initialize bifurcation buffer (capacity based on maze size)
  int_t buffer_capacity = (maze.dimensions.x * maze.dimensions.y) / 4;
  init_bifurcation_buffer(&state->bifurcations, buffer_capacity);

  // Allocate worker position tracking
  state->worker_positions =
      (worker_position_t *)calloc(num_workers, sizeof(worker_position_t));
  if (!state->worker_positions) {
    PERROR("Couldn't allocate worker positions array");
  }

  for (uint8_t i = 0; i < num_workers; i++) {
    state->worker_positions[i].is_active = false;
  }
}

// Cleans up solver state
void cleanup_solver_state(solver_state_t *state) {
  free_exploration_map(&state->explored);
  free(state->worker_positions);
  free_bifurcation_buffer(&state->bifurcations);
}

// Get available unexplored directions from current position (thread-safe)
// Assumes that caller already holds the region mutex for the current position
static direction_t get_available_directions(solver_state_t *state, vec2_t pos) {
  maze_t maze = state->maze;
  direction_t available = maze_at(maze, pos.x, pos.y).open_directions;
  direction_t result = 0;

  // Check each direction (mutex already held by caller)
  if (available & NORTH && !explored_at(state->explored, pos.x, pos.y - 1))
    result |= NORTH;
  if (available & SOUTH && !explored_at(state->explored, pos.x, pos.y + 1))
    result |= SOUTH;
  if (available & EAST && !explored_at(state->explored, pos.x + 1, pos.y))
    result |= EAST;
  if (available & WEST && !explored_at(state->explored, pos.x - 1, pos.y))
    result |= WEST;

  return result;
}

// Count number of directions in a direction mask
static int count_directions(direction_t dirs) {
  int count = 0;
  if (dirs & NORTH)
    count++;
  if (dirs & SOUTH)
    count++;
  if (dirs & EAST)
    count++;
  if (dirs & WEST)
    count++;
  return count;
}

static void *solver_worker(void *args) {
  worker_args_t *worker = (worker_args_t *)args;
  solver_state_t *state = worker->state;
  uint8_t worker_id = worker->worker_id;
  uint32_t speed = worker->speed;

  vec2_t current_position;
  direction_t entry_direction = 0;
  bool is_actively_exploring = false;
  int_t held_region_mutex = -1;

  if (worker_id == 0) {
    current_position = (vec2_t){0, 0};
    entry_direction = 0;
    is_actively_exploring = true;

    if (state->enable_visualization) {
      mark_worker_active_at_position(state, worker_id, current_position);
    }
    increment_active_workers(state);
  }

  while (true) {
    pthread_mutex_lock(&state->bifurcations.mutex);
    bool should_terminate = state->solution_found || state->shutdown;
    pthread_mutex_unlock(&state->bifurcations.mutex);

    if (should_terminate) {
      if (is_actively_exploring) {
        if (state->enable_visualization) {
          mark_worker_inactive(state, worker_id);
        }
        decrement_active_workers(state);
      }
      if (held_region_mutex != -1) {
        pthread_mutex_unlock(
            &state->explored.mutex_grid.mutexes[held_region_mutex]);
      }
      break;
    }

    if (!is_actively_exploring) {
      bifurcation_t next_work;
      bool work_acquired = false;

      pthread_mutex_lock(&state->bifurcations.mutex);

      while (state->bifurcations.count == 0 && !state->shutdown &&
             !state->solution_found) {
        int num_active_workers = state->active_workers;

        if (num_active_workers == 0) {
          // Deadlock detected: all workers idle and no work available
          state->shutdown = true;
          pthread_cond_broadcast(&state->bifurcations.work_available);
          break;
        }

        pthread_cond_wait(&state->bifurcations.work_available,
                          &state->bifurcations.mutex);
      }

      if (state->shutdown || state->solution_found) {
        pthread_mutex_unlock(&state->bifurcations.mutex);
        break;
      }

      if (state->bifurcations.count > 0) {
        next_work = state->bifurcations.data[state->bifurcations.head];
        state->bifurcations.head =
            (state->bifurcations.head + 1) % state->bifurcations.capacity;
        state->bifurcations.count--;
        work_acquired = true;
      }

      pthread_mutex_unlock(&state->bifurcations.mutex);

      if (work_acquired) {
        current_position = next_work.position;
        entry_direction = next_work.came_from;
        is_actively_exploring = true;

        if (state->enable_visualization) {
          mark_worker_active_at_position(state, worker_id, current_position);
        }
        increment_active_workers(state);

        // Release old region mutex if moving to a new region
        int_t new_region = get_mutex_index(state->explored, current_position.x,
                                           current_position.y);
        if (held_region_mutex != -1 && held_region_mutex != new_region) {
          pthread_mutex_unlock(
              &state->explored.mutex_grid.mutexes[held_region_mutex]);
          held_region_mutex = -1;
        }
      } else {
        continue;
      }
    }

    int_t region_mutex_idx = get_mutex_index(
        state->explored, current_position.x, current_position.y);
    if (held_region_mutex != region_mutex_idx) {
      if (held_region_mutex != -1) {
        pthread_mutex_unlock(
            &state->explored.mutex_grid.mutexes[held_region_mutex]);
      }
      pthread_mutex_lock(&state->explored.mutex_grid.mutexes[region_mutex_idx]);
      held_region_mutex = region_mutex_idx;
    }

    bool cell_already_explored =
        explored_at(state->explored, current_position.x, current_position.y);
    if (!cell_already_explored) {
      explored_at(state->explored, current_position.x, current_position.y) =
          true;

      // Save where im from for path reconstruction
      int_t cell_idx = state->explored.start + current_position.x +
                       current_position.y * state->explored.true_dimensions.x;
      state->explored.explored_from[cell_idx] = entry_direction;
    }

    if (cell_already_explored) {

      if (held_region_mutex != -1) {
        pthread_mutex_unlock(
            &state->explored.mutex_grid.mutexes[held_region_mutex]);
        held_region_mutex = -1;
      }

      if (state->enable_visualization) {
        mark_worker_inactive(state, worker_id);
      }
      decrement_active_workers(state);

      is_actively_exploring = false;
      continue;
    }

    bool reached_goal = (current_position.x == state->goal.x &&
                         current_position.y == state->goal.y);
    if (reached_goal) {
      // Release region mutex
      if (held_region_mutex != -1) {
        pthread_mutex_unlock(
            &state->explored.mutex_grid.mutexes[held_region_mutex]);
        held_region_mutex = -1;
      }

      if (state->enable_visualization) {
        mark_worker_inactive(state, worker_id);
      }

      pthread_mutex_lock(&state->bifurcations.mutex);
      state->solution_found = true;
      state->active_workers--;
      pthread_cond_broadcast(&state->bifurcations.work_available);
      pthread_mutex_unlock(&state->bifurcations.mutex);

      is_actively_exploring = false;
      break;
    }

    direction_t unexplored_directions =
        get_available_directions(state, current_position);
    int num_paths = count_directions(unexplored_directions);

    if (num_paths == 0) {
      if (held_region_mutex != -1) {
        pthread_mutex_unlock(
            &state->explored.mutex_grid.mutexes[held_region_mutex]);
        held_region_mutex = -1;
      }

      if (state->enable_visualization) {
        mark_worker_inactive(state, worker_id);
      }
      decrement_active_workers_and_signal(state);

      is_actively_exploring = false;

    } else if (num_paths == 1) {
      current_position =
          move_direction(current_position, unexplored_directions);
      entry_direction = opposite_direction(unexplored_directions);

      if (state->enable_visualization) {
        update_worker_position(state, worker_id, current_position);
      }

      if (state->enable_visualization) {
        usleep(speed);
      }

    } else {
      direction_t chosen_direction = 0;

      pthread_mutex_lock(&state->bifurcations.mutex);

      for (direction_t dir = NORTH; dir <= WEST; dir <<= 1) {
        if (unexplored_directions & dir) {
          if (chosen_direction == 0) {
            chosen_direction = dir;
          } else {
            vec2_t branch_position = move_direction(current_position, dir);

            if (state->bifurcations.count < state->bifurcations.capacity) {
              state->bifurcations.data[state->bifurcations.tail].position =
                  branch_position;
              state->bifurcations.data[state->bifurcations.tail].came_from =
                  opposite_direction(dir);
              state->bifurcations.tail =
                  (state->bifurcations.tail + 1) % state->bifurcations.capacity;
              state->bifurcations.count++;

              pthread_cond_signal(
                  &state->bifurcations.work_available); // Wake one idle worker
            }
          }
        }
      }

      pthread_mutex_unlock(&state->bifurcations.mutex);

      current_position = move_direction(current_position, chosen_direction);
      entry_direction = opposite_direction(chosen_direction);

      if (state->enable_visualization) {
        update_worker_position(state, worker_id, current_position);
      }

      if (state->enable_visualization) {
        usleep(speed); // ANIMATION SPEED
      }
    }
  }

  if (held_region_mutex != -1) {
    pthread_mutex_unlock(
        &state->explored.mutex_grid.mutexes[held_region_mutex]);
  }
  free(worker);
  return NULL;
}

void solve_maze(maze_t maze, uint8_t num_workers,
                bool enable_iterative_visualization, uint32_t speed) {

  wprintf(L"Starting maze solver with %d workers\n", num_workers);
  wprintf(L"Maze dimensions: %d x %d\n", maze.dimensions.x, maze.dimensions.y);
  wprintf(L"Goal: (%d, %d)\n", maze.dimensions.x - 1, maze.dimensions.y - 1);

  if (!enable_iterative_visualization) {
    wprintf(L"\n=== INITIAL MAZE ===\n");
    print_maze(maze);
    wprintf(L"\n");
  }

  solver_state_t state;
  init_solver_state(&state, maze, num_workers, enable_iterative_visualization);
  state.speed = speed;

  pthread_t *threads = (pthread_t *)malloc(num_workers * sizeof(pthread_t));
  if (!threads) {
    PERROR("Couldn't allocate threads array");
  }

  pthread_t viz_thread;
  if (enable_iterative_visualization) {
    pthread_create(&viz_thread, NULL, visualizer_thread, (void *)&state);
  }
  usleep(200);
  for (uint8_t i = 0; i < num_workers; i++) {
    worker_args_t *args = (worker_args_t *)malloc(sizeof(worker_args_t));
    if (!args) {
      PERROR("Couldn't allocate worker args for thread %d", i);
    }

    args->state = &state;
    args->worker_id = i;
    args->speed = speed;

    pthread_create(&threads[i], NULL, solver_worker, (void *)args);
  }

  for (uint8_t i = 0; i < num_workers; i++) {
    pthread_join(threads[i], NULL);
  }

  if (enable_iterative_visualization) {
    pthread_join(viz_thread, NULL);

    // Clear screen and show final result
    wprintf(L"\033[2J\033[H");
  }

  if (state.solution_found) {
    wprintf(L"\n✓ Solution found!\n");
    wprintf(L"\n=== SOLUTION PATH ===\n");
    print_maze_with_solution(&state);
  } else {
    wprintf(L"\n✗ No solution found.\n");
    wprintf(L"\n=== EXPLORED CELLS ===\n");
    print_maze_explored(&state);
  }
  wprintf(L"\n");

  free(threads);
  cleanup_solver_state(&state);
}
