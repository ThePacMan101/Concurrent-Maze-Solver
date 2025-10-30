#include "visualization.h"
#include "special_characters.h"
#include <unistd.h>
#include <wchar.h>


// Update worker visualization state (marks worker as active at given position)
void mark_worker_active_at_position(solver_state_t *state, uint8_t worker_id, vec2_t position) {
    pthread_mutex_lock(&state->viz_mutex);
    state->worker_positions[worker_id].position = position;
    state->worker_positions[worker_id].is_active = true;
    pthread_mutex_unlock(&state->viz_mutex);
}

// Mark worker as inactive in visualization
void mark_worker_inactive(solver_state_t *state, uint8_t worker_id) {
    pthread_mutex_lock(&state->viz_mutex);
    state->worker_positions[worker_id].is_active = false;
    pthread_mutex_unlock(&state->viz_mutex);
}

// Update worker position (keeping active state)
void update_worker_position(solver_state_t *state, uint8_t worker_id, vec2_t position) {
    pthread_mutex_lock(&state->viz_mutex);
    state->worker_positions[worker_id].position = position;
    pthread_mutex_unlock(&state->viz_mutex);
}

void print_maze_explored(solver_state_t *state) {
    maze_t maze = state->maze;
    exploration_map_t *exp = &state->explored;
    
    for(int y = 0; y < maze.dimensions.y; y++) {
        for(int x = 0; x < maze.dimensions.x; x++) {
            maze_vertex_t vertex = maze_at(maze, x, y);
            spchar_t ch = get_box_char(vertex.open_directions);
            
            // Print horizontal connector if not first column
            if(x > 0) {
                spchar_t middle_ch;
                if((maze_at(maze, x-1, y).open_directions & EAST) && 
                   (maze_at(maze, x, y).open_directions & WEST)) {
                    middle_ch = get_box_char(EAST|WEST);
                } else {
                    middle_ch = get_box_char(0);
                }
                
                // Color if explored
                bool explored = explored_at(*exp, x, y) || explored_at(*exp, x-1, y);
                if(explored) wprintf(L"\033[36m");  // ANSI cyan
                print_spchar(middle_ch);
                if(explored) wprintf(L"\033[0m");   // ANSI reset
            }
            
            // Color the cell based on exploration status
            bool is_explored = explored_at(*exp, x, y);
            
            // Mark start and goal specially
            if(x == 0 && y == 0) {
                wprintf(L"\033[42m\033[1m");  // Green background + bold
            } else if(x == state->goal.x && y == state->goal.y) {
                wprintf(L"\033[41m\033[1m");  // Red background + bold
            } else if(is_explored) {
                wprintf(L"\033[36m");  // Cyan
            }
            
            print_spchar(ch);
            wprintf(L"\033[0m");  // Reset
        }
        print_spchar(L'\n');
    }
}

// Print maze with solution path highlighted
void print_maze_with_solution(solver_state_t *state) {
    maze_t maze = state->maze;
    exploration_map_t *exp = &state->explored;
    
    // Build solution path by tracing back from goal to start
    bool *is_on_path = (bool*) calloc(maze.true_dimensions.x * maze.true_dimensions.y, sizeof(bool));
    
    // Trace back from goal to start
    vec2_t current = state->goal;
    while(!(current.x == 0 && current.y == 0)) {
        int_t idx = exp->start + current.x + current.y * exp->true_dimensions.x;
        is_on_path[idx] = true;
        
        direction_t came_from = exp->explored_from[idx];
        if(came_from == 0) break; // Shouldn't happen if solution exists
        
        // Move to previous cell
        current = move_direction(current, came_from);
    }
    // Mark start
    is_on_path[exp->start] = true;
    
    for(int y = 0; y < maze.dimensions.y; y++) {
        for(int x = 0; x < maze.dimensions.x; x++) {
            maze_vertex_t vertex = maze_at(maze, x, y);
            spchar_t ch = get_box_char(vertex.open_directions);
            
            // Print horizontal connector if not first column
            if(x > 0) {
                spchar_t middle_ch;
                if((maze_at(maze, x-1, y).open_directions & EAST) && 
                   (maze_at(maze, x, y).open_directions & WEST)) {
                    middle_ch = get_box_char(EAST|WEST);
                } else {
                    middle_ch = get_box_char(0);
                }
                
                // Highlight if on solution path
                int_t idx = exp->start + x + y * exp->true_dimensions.x;
                int_t idx_left = exp->start + (x-1) + y * exp->true_dimensions.x;
                bool on_path = is_on_path[idx] || is_on_path[idx_left];
                
                if(on_path) wprintf(L"\033[33m\033[1m");  // Yellow + bold
                print_spchar(middle_ch);
                if(on_path) wprintf(L"\033[0m");  // Reset
            }
            
            // Check if on solution path
            int_t idx = exp->start + x + y * exp->true_dimensions.x;
            bool on_path = is_on_path[idx];
            
            // Mark start and goal specially
            if(x == 0 && y == 0) {
                wprintf(L"\033[42m\033[1m");  // Green background + bold
            } else if(x == state->goal.x && y == state->goal.y) {
                wprintf(L"\033[41m\033[1m");  // Red background + bold
            } else if(on_path) {
                wprintf(L"\033[33m\033[1m");  // Yellow + bold
            }
            
            print_spchar(ch);
            wprintf(L"\033[0m");  // Reset
        }
        print_spchar(L'\n');
    }
    
    free(is_on_path);
}

// Print maze with live worker positions (for animation)
void print_maze_animated(solver_state_t *state) {
    maze_t maze = state->maze;
    exploration_map_t *exp = &state->explored;
    
    // Worker background color codes (different bright backgrounds for each worker)
    const wchar_t* worker_bg_colors[] = {
        L"\033[105m",  // Bright magenta background
        L"\033[106m",  // Bright cyan background
        L"\033[103m",  // Bright yellow background
        L"\033[102m",  // Bright green background
        L"\033[104m",  // Bright blue background
        L"\033[101m",  // Bright red background
        L"\033[45m",   // Magenta background
        L"\033[46m",   // Cyan background
    };
    int num_colors = 8;
    
    for(int y = 0; y < maze.dimensions.y; y++) {
        for(int x = 0; x < maze.dimensions.x; x++) {
            maze_vertex_t vertex = maze_at(maze, x, y);
            spchar_t ch = get_box_char(vertex.open_directions);
            
            // Check if any worker is at this position
            int worker_here = -1;
            pthread_mutex_lock(&state->viz_mutex);
            for (uint8_t w = 0; w < state->num_workers; w++) {
                if (state->worker_positions[w].is_active &&
                    state->worker_positions[w].position.x == x &&
                    state->worker_positions[w].position.y == y) {
                    worker_here = w;
                    break;
                }
            }
            pthread_mutex_unlock(&state->viz_mutex);
            
            // Print horizontal connector if not first column
            if(x > 0) {
                spchar_t middle_ch;
                if((maze_at(maze, x-1, y).open_directions & EAST) && 
                   (maze_at(maze, x, y).open_directions & WEST)) {
                    middle_ch = get_box_char(EAST|WEST);
                } else {
                    middle_ch = get_box_char(0);
                }
                
                // Check if worker is on left cell (for coloring the connector)
                int worker_left = -1;
                pthread_mutex_lock(&state->viz_mutex);
                for (uint8_t w = 0; w < state->num_workers; w++) {
                    if (state->worker_positions[w].is_active &&
                        state->worker_positions[w].position.x == x-1 &&
                        state->worker_positions[w].position.y == y) {
                        worker_left = w;
                        break;
                    }
                }
                pthread_mutex_unlock(&state->viz_mutex);
                
                // Color connector based on worker presence
                if(worker_here >= 0 || worker_left >= 0) {
                    int worker_id = (worker_here >= 0) ? worker_here : worker_left;
                    wprintf(L"%ls\033[1m", worker_bg_colors[worker_id % num_colors]);
                } else {
                    // Color if explored
                    bool explored = explored_at(*exp, x, y) || explored_at(*exp, x-1, y);
                    if(explored) wprintf(L"\033[2m\033[36m");  // Dim cyan for explored
                }
                print_spchar(middle_ch);
                wprintf(L"\033[0m");
            }
            
            // Color the cell
            bool is_explored = explored_at(*exp, x, y);
            
            if(x == 0 && y == 0) {
                wprintf(L"\033[42m\033[1m\033[30m");  // Green background + bold + black text (start)
            } else if(x == state->goal.x && y == state->goal.y) {
                wprintf(L"\033[41m\033[1m\033[97m");  // Red background + bold + white text (goal)
            } else if(worker_here >= 0) {
                wprintf(L"%ls\033[1m\033[30m", worker_bg_colors[worker_here % num_colors]);  // Worker background + bold + black text
            } else if(is_explored) {
                wprintf(L"\033[2m\033[36m");  // Dim cyan for explored
            }
            
            print_spchar(ch);
            wprintf(L"\033[0m");
        }
        print_spchar(L'\n');
    }
}

// Visualization thread function
void* visualizer_thread(void *args) {
    solver_state_t *state = (solver_state_t*) args;
    
    // Clear screen initially
    wprintf(L"\033[2J\033[H");
    
    int frame = 0;
    while (true) {
        pthread_mutex_lock(&state->bifurcations.mutex);
        bool done = state->solution_found || state->shutdown;
        int active = state->active_workers;
        pthread_mutex_unlock(&state->bifurcations.mutex);
        
        if (done) break;
        
        // Clear screen and move cursor to top
        wprintf(L"\033[H"); // Move to top (don't clear, just overwrite)
        
        // Print header
        wprintf(L"╔════════════════════════════════════════════════════════╗\n");
        wprintf(L"║             CONCURRENT MAZE SOLVER                     ║\n");
        wprintf(L"╚════════════════════════════════════════════════════════╝\n");
        wprintf(L"Active workers: %d/%d  |  Frame: %d\n", active, state->num_workers, frame++);
        wprintf(L"Legend: ");
        wprintf(L"\033[42m\033[30m▓\033[0m=Start  ");
        wprintf(L"\033[41m\033[97m▓\033[0m=Goal  ");
        wprintf(L"\033[2m\033[36m▓\033[0m=Explored  \n\n");
        // wprintf(L"Workers:");
        // wprintf(L"\033[105m\033[30m▓\033[0m");
        // wprintf(L"\033[106m\033[30m▓\033[0m");
        // wprintf(L"\033[103m\033[30m▓\033[0m");
        // wprintf(L"\033[102m\033[30m▓\033[0m");
        // wprintf(L"\033[104m\033[30m▓\033[0m");
        // wprintf(L"\033[101m\033[97m▓\033[0m\n\n");
        
        // Print maze with workers
        print_maze_animated(state);
        
        // Small delay to control animation speed
        usleep(5000); // 50ms = 20 FPS
    }
    
    return NULL;
}
