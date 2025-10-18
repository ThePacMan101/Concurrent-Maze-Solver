#include "maze.h"
#include "special_characters.h"
#include "common.h"

static void alloc_maze(maze_t *maze,int_t x, int_t y){
    maze->dimensions.x = x;
    maze->dimensions.y = y;
    maze->data = (maze_vertex_t*) calloc(x*y,sizeof(maze_vertex_t));
    if(!maze->data) 
        PERROR("Couldn\'t allocate space for the maze with size: %d x %d",x,y);

    return;
}

void print_maze(maze_t maze){
    for(int y = 0 ; y < maze.dimensions.y ; ++y){
        for(int x = 0 ; x < maze.dimensions.x ; ++x){
            maze_vertex_t vertex = maze_at(maze,x,y);
            spchar_t ch = get_box_char(vertex.open_directions);
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
    generator_maze_at(dim_x-1,dim_y-1) = -1;

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
        
        generator_maze[origin] = -1;
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
    if(blueprint[x+y*dim_x] == -1) return 0; // origin, points to no one
    if(x+1 < dim_x && blueprint[x+y*dim_x] == (x+1)+y*dim_x) return EAST;
    if(x-1 >= 0    && blueprint[x+y*dim_x] == (x-1)+y*dim_x) return WEST;
    if(y+1 < dim_y && blueprint[x+y*dim_x] == x+(y+1)*dim_x) return SOUTH;
    if(y-1 >= 0    && blueprint[x+y*dim_x] == x+(y-1)*dim_x) return NORTH;
    return -1;
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