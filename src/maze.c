#include "maze.h"
#include "special_characters.h"
#include "common.h"
#include <pthread.h>

static void alloc_maze(maze_t *maze,int_t dim_x, int_t dim_y){
    maze->dimensions = (vec2_t){dim_x,dim_y};
    maze->true_dimensions = maze->dimensions;
    maze->start = 0;
    maze->data = (maze_vertex_t*) calloc(dim_x*dim_y,sizeof(maze_vertex_t));
    if(!maze->data) 
        PERROR("Couldn\'t allocate space for the maze with size: %d x %d",dim_x,dim_y);

    return;
}

maze_t get_sub_maze(maze_t maze,int_t start_x,int_t start_y,int_t end_x,int_t end_y){
    maze_t sub = maze;
    sub.start = maze.start + start_x + start_y*maze.true_dimensions.x;
    sub.dimensions.x = end_x - start_x;
    sub.dimensions.y = end_y - start_y;
    if(sub.dimensions.x<=0 || sub.dimensions.y<=0){
        free(maze.data);
        PERROR("invalid Sub-maze with dims %d x %d",sub.dimensions.x,sub.dimensions.y);
    } 
    return sub;
}

void print_maze(maze_t maze){
    for(int y = 0 ; y < maze.dimensions.y ; ++y){
        for(int x = 0 ; x < maze.dimensions.x ; ++x){
            maze_vertex_t vertex = maze_at(maze,x,y);
            spchar_t ch = get_box_char(vertex.open_directions);
            
            if(x>0){
                
                if((maze_at(maze,x-1,y).open_directions&EAST) && (maze_at(maze,x,y).open_directions&WEST)){
                    spchar_t middle_ch = get_box_char(EAST|WEST);
                    print_spchar(middle_ch);
                }else{
                    spchar_t middle_ch = get_box_char(0);
                    print_spchar(middle_ch);
                }
                
            }

            print_spchar(ch);
        }
        printf("\n");
    }
}

static direction_t random_direction(direction_t available){
    uint8_t number_of_available_directions = ((available>>0)&1) + ((available>>1)&1) + ((available>>2)&1) + ((available>>3)&1);
    uint8_t random = rand()%number_of_available_directions + 1; 
    // if there is 2 available directions, let's say East and West, available = <WSEN> = <1010>
    // in this case, number of available directions is 2, so the random number is in {1, 2}
    // If it's 1, the chosen direction is the first available from North to West, 
    // so in this case, it's East: <0010>
    // If it's 2, the chosen direction is the second available from North to West, 
    // so in this case, it's West: <1000>

    // The strategy is: start at North, and go towards West, whenever the bit in the direction
    // I'm looking is also 1 in the available directions, then I check if my random number is 1
    // If it's greater then 1, I lower it by one and continue, else I choose that direction
    for(direction_t direction = NORTH; direction <= WEST; direction<<=1){
        if(available&direction){
            if(random == 1)
                return direction;
            else{
                random --;
            }
        }
    }
    return available;
}

// credits:
// https://www.youtube.com/watch?v=zbXKcDVV4G0
static int_t * random_maze_blueprint(int_t dim_x, int_t dim_y, uint64_t number_of_iterations){
#define generator_maze_position(_x,_y) (_x)+dim_x*(_y)
#define generator_maze_at(_x,_y) generator_maze[generator_maze_position(_x,_y)]
#define origin_x (origin%dim_x)
#define origin_y (origin/dim_x )
 
    int_t *generator_maze = (int_t*)calloc(sizeof(int_t),(dim_y*dim_x));
    // everyone points to the right
    for (int_t y = 0 ; y < dim_y ;++y){
        for(int_t x = 0 ; x < dim_x-1 ; ++x){
            generator_maze_at(x,y) = x+dim_x*y + 1;
        }
    }

    // the rightmost column points down
    for (int_t y = 0 ; y < dim_y -1 ; ++y)
        generator_maze_at(dim_x-1,y) = generator_maze_position(dim_x-1,y)+dim_x;

    // the origin is the lower right of the maze
    int_t origin =  dim_x*dim_y -1; 
    // the pointer in the origin points to NULL (-1)
    generator_maze_at(dim_x-1,dim_y-1) = (int_t)-1;

    // the algorithm has the following steps:
    /*
        1. the origin points to a random neighbor
        2. the neighbor node becomes the new origin
        3. the new origin points to NULL (-1)
        4. go back to (1) unless you want to finish, anytime
    */
    for(int_t i = 0 ; i < number_of_iterations ; ++i){
        direction_t available = NORTH|WEST|SOUTH|EAST;
        direction_t prohibited = 0;

        if(origin_x == 0) 
            prohibited|=WEST;
        if(origin_x == dim_x-1) 
            prohibited|=EAST;
        if(origin_y == 0)
            prohibited |=NORTH;
        if(origin_y == dim_y-1)
            prohibited |= SOUTH;
        
        available&=~prohibited;

        direction_t chosen_direction = random_direction(available);

        switch (chosen_direction){
            case NORTH :

                generator_maze[origin] = origin - dim_x;
                origin = generator_maze[origin];

                break; 
            case WEST  :

                generator_maze[origin] = origin -1;
                origin = generator_maze[origin];

                break; 
            case SOUTH : 

                generator_maze[origin] = origin + dim_x;
                origin = generator_maze[origin];

                break; 
            case EAST  : 

                generator_maze[origin] = origin +1;
                origin = generator_maze[origin];

                break;
            default:
                break;
                
        }
        
        generator_maze[origin] = (int_t)-1;
    }

    return generator_maze;
#undef origin_x
#undef origin_y
#undef generator_maze_position
#undef generator_maze_at
}

static direction_t get_pointing_direction(maze_t *maze,int_t* blueprint,int_t x, int_t y){
    int_t dim_x = maze->dimensions.x;
    int_t dim_y = maze->dimensions.y;
    if(blueprint[x+y*dim_x] == (int_t)-1) return 0; // origin, points to no one
    if(x+1 < dim_x && blueprint[x+y*dim_x] == (x+1)+y*dim_x) return EAST;
    if(x-1 >= 0    && blueprint[x+y*dim_x] == (x-1)+y*dim_x) return WEST;
    if(y+1 < dim_y && blueprint[x+y*dim_x] == x+(y+1)*dim_x) return SOUTH;
    if(y-1 >= 0    && blueprint[x+y*dim_x] == x+(y-1)*dim_x) return NORTH;
    return 0;
}

static void build_maze_blueprint(maze_t *maze,int_t* blueprint){
    int_t dim_x = maze->dimensions.x;
    int_t dim_y = maze->dimensions.y;
    for(int_t y = 0 ; y < dim_y ; ++y){
        for(int_t x = 0 ; x < dim_x ; ++x){
            direction_t pointing_towards = get_pointing_direction(maze,blueprint,x,y);
            switch (pointing_towards){
                case NORTH:
                    maze_at(*maze,x,y).open_directions |=NORTH;
                    maze_at(*maze,x,y-1).open_directions |=SOUTH;
                break;
                case SOUTH:
                    maze_at(*maze,x,y).open_directions |=SOUTH;
                    maze_at(*maze,x,y+1).open_directions |=NORTH;
                break;
                case WEST:
                    maze_at(*maze,x,y).open_directions |=WEST;
                    maze_at(*maze,x-1,y).open_directions |=EAST;
                break;
                case EAST:
                    maze_at(*maze,x,y).open_directions |=EAST;
                    maze_at(*maze,x+1,y).open_directions |=WEST;
                break;
                default:
                break;
            }

        }
    }
    free(blueprint);
}

maze_t generate_random_maze(int_t dim_x,int_t dim_y,uint64_t number_of_iterations){
    maze_t maze;
    maze.dimensions.x=dim_x;
    maze.dimensions.y=dim_y; 
    alloc_maze(&maze,dim_x,dim_y);
    int_t * blueprint =random_maze_blueprint(dim_x,dim_y,number_of_iterations);
    build_maze_blueprint(&maze,blueprint);
    return maze;
}

// Splits the maze in two parts, either horizontally of vertically
static void split_maze_in_half(maze_t origin, maze_t *target_1,maze_t *target_2, bool horizontal){
    int_t dim_x = origin.dimensions.x;
    int_t dim_y = origin.dimensions.y;

    if(horizontal){
        *target_1 = get_sub_maze(origin,0,0,dim_x/2,dim_y);
        *target_2 = get_sub_maze(origin,dim_x/2,0,dim_x,dim_y);
    }else{
        *target_1 = get_sub_maze(origin,0,0,dim_x,dim_y/2);
        *target_2 = get_sub_maze(origin,0,dim_y/2,dim_x,dim_y);    
    }
}


// splits a maze into parts, each one is stored in a slot of the target array
// parts store in target are just new references to the same memory from the 
// original maze. Must be power of 2 parts.
static uint8_t free_slot = 0 ;
static void _recursive_split_maze(maze_t maze ,maze_t *target,uint8_t parts);
void split_maze(maze_t maze ,maze_t *target,uint8_t parts){
    if(parts != 1<<((uint8_t)log2l(parts))) PERROR("Can't split maze in %d parts, must be power of 2.",parts);
    free_slot = 0;
    _recursive_split_maze(maze, target, parts);
    free_slot = 0;
}
static void _recursive_split_maze(maze_t maze ,maze_t *target,uint8_t parts){
    // Always slices the maze in half through the bigger side
    int_t max_side = (maze.dimensions.x>maze.dimensions.y)?maze.dimensions.x:maze.dimensions.y;
    bool should_cut_horizontaly = max_side==maze.dimensions.x;
    
    // Cuts into parts A and B
    maze_t A,B;
    split_maze_in_half(maze,&A,&B,should_cut_horizontaly);
    
    // If the amount of parts the maze is cut into is equal to two, then
    // store the parts on the array, then end.
    if (parts == 2){
        target[free_slot++] = A;
        target[free_slot++] = B;
        return;
    }

    // If the amount of parts is different from two, you split each part, as needed
    // and then, end.
    _recursive_split_maze(A , target, parts/2);
    _recursive_split_maze(B , target, parts/2);
}

typedef struct{
    uint64_t number_of_iterations;
    maze_t sub_maze;
    uint8_t id;
} _generate_random_maze_thread_args_t;
static pthread_mutex_t mutex;
void * _generate_random_maze_thread(void*void_args);
maze_t generate_random_maze_parallel(int_t dim_x,int_t dim_y,uint64_t number_of_iterations,uint8_t workers){
    // if only one worker, just use sequential generation
    if(workers == 1)
        return generate_random_maze(dim_x,dim_y,number_of_iterations);
    
    if(log2l(workers) != (int_t)log2l(workers)) 
        PERROR("Couldn't generate generate random maze in paralel, only power of two worker threads accepted");

    // create and allocate an empty maze, and a array of sub_mazes
    maze_t maze;
    alloc_maze(&maze,dim_x,dim_y);
    if(!maze.data) 
        PERROR("Couldn't allocate space for maze with size %d x %d",dim_x,dim_y);
    maze_t *sub_mazes = calloc(workers,sizeof(maze_t));
    if(!sub_mazes) 
        PERROR("Couldn't allocate space for submazes while generating maze in parallel. Number of Workers: %d",workers);
    
    // split the main maze so that each 
    // element of the sub_mazes array 
    // *references* some part of the maze
    split_maze(maze,sub_mazes,workers);


    // allocate threads

    pthread_mutex_init(&mutex,NULL);

    pthread_t * tid = calloc(workers,sizeof(pthread_t));
    if(!tid) PERROR("Couldn't allocate space for threads while generating maze in parallel.");

    for(uint8_t i = 0 ; i < workers ; ++i){
        _generate_random_maze_thread_args_t *args;
        size_t args_size = sizeof(_generate_random_maze_thread_args_t);
        args = malloc(args_size);
        if(!args) PERROR("Couldn't allocate space for arguments in thread %d while generating maze in parallel.",i);
        
        args->number_of_iterations = number_of_iterations;
        args->sub_maze = sub_mazes[i];
        args->id = i;
        
        pthread_create(&tid[i],NULL,_generate_random_maze_thread,(void*)args);
    }

    // join threads
    for(uint8_t i = 0 ; i < workers ; ++i){
        pthread_join(tid[i],(void**)NULL);
    }


    // Now that we generated the "insides" of each sub maze, we 
    // must connect them, to do so, we simply TODO



    // free memory and return maze
    pthread_mutex_destroy(&mutex);
    free(tid);
    free(sub_mazes);
    return maze;
}


void * _generate_random_maze_thread(void*void_args){
    _generate_random_maze_thread_args_t *args;
    args = (_generate_random_maze_thread_args_t *)void_args;

    int_t dim_x,dim_y;
    dim_x = args->sub_maze.dimensions.x;
    dim_y = args->sub_maze.dimensions.y;
    uint64_t number_of_iterations = args->number_of_iterations;

    // hashing function on thread id and current time for seed generation
    // we do it with a mutex, to assure time is different on each one
    pthread_mutex_lock(&mutex);
    uint8_t seed = (100*args->id)*time(NULL)+time(NULL)%119;
    srand(seed);
    pthread_mutex_unlock(&mutex);

    int_t *blueprint = random_maze_blueprint(dim_x,dim_y,number_of_iterations);
    build_maze_blueprint(&(args->sub_maze),blueprint);
    
    
    
    free(args);

    return (void*)NULL;
}