#include "maze.h"
#include "special_characters.h"
#include "common.h"


void print_direction(direction_t direction){
#define P_DIR(D) case D: printf(#D);break;
    switch (direction) {
        P_DIR(NORTH);
        P_DIR(SOUTH);
        P_DIR(EAST);
        P_DIR(WEST);

        P_DIR(NORTH|EAST);
        P_DIR(NORTH|SOUTH);
        P_DIR(NORTH|WEST);
        P_DIR(EAST|SOUTH);
        P_DIR(EAST|WEST);
        P_DIR(SOUTH|WEST);

        P_DIR(NORTH|EAST|SOUTH);
        P_DIR(NORTH|EAST|WEST);
        P_DIR(NORTH|SOUTH|WEST);
        P_DIR(EAST|SOUTH|WEST);

        P_DIR(NORTH|EAST|SOUTH|WEST);
        default:
        break;
    }
#undef P_DIR
}



void alloc_maze(maze_t *maze,int_t dim_x, int_t dim_y){
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
        print_spchar(L'\n');
    }
}



direction_t random_direction(direction_t available){
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



// simple buble sort to sort submazes in array based on maze.start value
// which means it goest left to right, top to bottom, in that order.
void sort_sub_mazes(maze_t* sub_mazes, uint8_t parts){
    bool sorted;
    for(int i = 0 ; i < parts-1 ; ++i){
        sorted = true;
        for(int j = 0; j < parts-1-i ;++j){
            if(sub_mazes[j].start > sub_mazes[j+1].start){
                maze_t aux = sub_mazes[j];
                sub_mazes[j] = sub_mazes[j+1];
                sub_mazes[j+1] = aux;
                sorted = false;
            }
        }
        if(sorted) break;
    }
}



// splits a maze into parts, each one is stored in a slot of the target array
// parts store in target are just new references to the same memory from the 
// original maze. Must be power of 2 parts. The sub_mazes array is sorted keyed 
// at maze.start, which means it goes left to right, top to bottom, in that order.
static uint8_t free_slot = 0 ;
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
static void _recursive_split_maze(maze_t maze ,maze_t *target,uint8_t parts){
    // Always slices the maze in half through the bigger side
    int_t max_side = (maze.dimensions.x>maze.dimensions.y)?maze.dimensions.x:maze.dimensions.y;
    bool should_cut_horizontaly = max_side==maze.dimensions.x && max_side!=maze.dimensions.y;
    
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
void split_maze(maze_t maze ,maze_t *target,uint8_t parts){
    if(parts != 1<<((uint8_t)log2l(parts))) PERROR("Can't split maze in %d parts, must be power of 2.",parts);
    free_slot = 0;
    _recursive_split_maze(maze, target, parts);
    sort_sub_mazes(target,parts);
    free_slot = 0;
}