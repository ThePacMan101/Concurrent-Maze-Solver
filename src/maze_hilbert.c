#include "common.h"
#include "maze.h"
#include "special_characters.h"
#include <inttypes.h>


// Hilbert Curve:
//
// order 1:
// ┌──┐ 
// │  │ 
//
// order 2:
// ┌──┐  ┌──┐    
// │  └──┘  │    
// └──┐  ┌──┘  
// ───┘  └───    



typedef struct{
    uint64_t *data;
    uint64_t start;
    uint64_t original_side;
    uint64_t side;
}hilbert_curve_t;
#define hc_at(hc,_x,_y)  ((hc).data[(hc).start+(_x)+(_y)*(hc).original_side])
#define hc_pos(hc,_x,_y) ((_x)+(_y)*(hc).original_side)


static void print_hilbert_curve(hilbert_curve_t hc){
    for(int i = 0 ; i < hc.side ; ++i){
        for(int j = 0 ; j < hc.side ; ++j){
            printf("%03"PRIu64" ", hc_at(hc,j,i));
        }                
        printf("\n");
    }
}

static hilbert_curve_t hilbert_curve_of_order(uint8_t order);

// there are eight different variations of the 
// hillbert curve fill, based on the
// order the quadrants that are filled 
// and the orientation:

// A:
// ↓ •     
// → ↑ 
static void hillbert_curve_fill_A(hilbert_curve_t hc,uint64_t start_index);

// B:
// → •     
// ↑ ←    
static void hillbert_curve_fill_B(hilbert_curve_t hc,uint64_t start_index);

// C:
// • ←
// → ↑
static void hillbert_curve_fill_C(hilbert_curve_t hc,uint64_t start_index);

// D:
// • ↓
// ↑ ←      
static void hillbert_curve_fill_D(hilbert_curve_t hc,uint64_t start_index);

// E:
// → ↓
// • ←  
static void hillbert_curve_fill_E(hilbert_curve_t hc,uint64_t start_index);

// F:
// ↓ ←
// • ↑ 
static void hillbert_curve_fill_F(hilbert_curve_t hc,uint64_t start_index);

// G:
// → ↓
// ↑ •     
static void hillbert_curve_fill_G(hilbert_curve_t hc,uint64_t start_index);

// H:
// ↓ ←
// → •        
static void hillbert_curve_fill_H(hilbert_curve_t hc,uint64_t start_index);



static void hillbert_curve_fill_A(hilbert_curve_t hc,uint64_t start_index){
    if(hc.side==1){
        hc_at(hc,0,0) = start_index;
        return;
    }
    uint64_t half_side = hc.side/2;
    uint64_t quadrant_size =  half_side*half_side;
    
// A:
// ↓ •     
// → ↑ 
// E -> A -> A -> B

    hilbert_curve_t TL,BL,BR,TR;
    uint64_t TL_start,BL_start,BR_start,TR_start;

    TL_start = start_index+0*quadrant_size;
    BL_start = start_index+1*quadrant_size;
    BR_start = start_index+2*quadrant_size;
    TR_start = start_index+3*quadrant_size;
    
    TL=hc;
    TL.side  = half_side;
    hillbert_curve_fill_E(TL,TL_start);
    

    BL=hc;
    BL.side=half_side;
    BL.start = hc.start+half_side*hc.original_side;
    hillbert_curve_fill_A(BL,BL_start);
    

    BR=hc;
    BR.side  = half_side;
    BR.start = hc.start+half_side*hc.original_side+half_side;
    hillbert_curve_fill_A(BR,BR_start);
    

    TR=hc;
    TR.side  = half_side;
    TR.start = hc.start + half_side;
    hillbert_curve_fill_B(TR,TR_start);
    
}

static void hillbert_curve_fill_B(hilbert_curve_t hc,uint64_t start_index){
    if(hc.side==1){
        hc_at(hc,0,0) = start_index;
        return;
    }
    uint64_t half_side = hc.side/2;
    uint64_t quadrant_size =  half_side*half_side;
    
// B:
// → •     
// ↑ ←     
// F -> B -> B -> A 

    hilbert_curve_t TL,BL,BR,TR;
    uint64_t TL_start,BL_start,BR_start,TR_start;

    BR_start = start_index+0*quadrant_size;
    BL_start = start_index+1*quadrant_size;
    TL_start = start_index+2*quadrant_size;
    TR_start = start_index+3*quadrant_size;
    
    BR=hc;
    BR.side  = half_side;
    BR.start = hc.start+half_side*hc.original_side+half_side;
    hillbert_curve_fill_F(BR,BR_start);
    

    BL=hc;
    BL.side=half_side;
    BL.start = hc.start+half_side*hc.original_side;
    hillbert_curve_fill_B(BL,BL_start);
    

    TL=hc;
    TL.side  = half_side;
    hillbert_curve_fill_B(TL,TL_start);
    

    TR=hc;
    TR.side  = half_side;
    TR.start = hc.start + half_side;
    hillbert_curve_fill_A(TR,TR_start);
    
}

static void hillbert_curve_fill_C(hilbert_curve_t hc,uint64_t start_index){
    if(hc.side==1){
        hc_at(hc,0,0) = start_index;
        return;
    }
    uint64_t half_side = hc.side/2;
    uint64_t quadrant_size =  half_side*half_side;
    
// C:
// • ←
// → ↑  
// G -> C -> C -> D  

    hilbert_curve_t TL,BL,BR,TR;
    uint64_t TL_start,BL_start,BR_start,TR_start;

    BL_start = start_index+0*quadrant_size;
    BR_start = start_index+1*quadrant_size;
    TR_start = start_index+2*quadrant_size;
    TL_start = start_index+3*quadrant_size;
    
    BL=hc;
    BL.side=half_side;
    BL.start = hc.start+half_side*hc.original_side;
    hillbert_curve_fill_G(BL,BL_start);
    

    BR=hc;
    BR.side  = half_side;
    BR.start = hc.start+half_side*hc.original_side+half_side;
    hillbert_curve_fill_C(BR,BR_start);
    

    TR=hc;
    TR.side  = half_side;
    TR.start = hc.start + half_side;
    hillbert_curve_fill_C(TR,TR_start);
    

    TL=hc;
    TL.side  = half_side;
    hillbert_curve_fill_D(TL,TL_start);
    
}

static void hillbert_curve_fill_D(hilbert_curve_t hc,uint64_t start_index){
    if(hc.side==1){
        hc_at(hc,0,0) = start_index;
        return;
    }
    uint64_t half_side = hc.side/2;
    uint64_t quadrant_size =  half_side*half_side;
    
// D:
// • ↓
// ↑ ←
// H -> D -> D -> C   

    hilbert_curve_t TL,BL,BR,TR;
    uint64_t TL_start,BL_start,BR_start,TR_start;

    TR_start = start_index+0*quadrant_size;
    BR_start = start_index+1*quadrant_size;
    BL_start = start_index+2*quadrant_size;
    TL_start = start_index+3*quadrant_size;
    
    TR=hc;
    TR.side  = half_side;
    TR.start = hc.start + half_side;
    hillbert_curve_fill_H(TR,TR_start);
    

    BR=hc;
    BR.side  = half_side;
    BR.start = hc.start+half_side*hc.original_side+half_side;
    hillbert_curve_fill_D(BR,BR_start);
    

    BL=hc;
    BL.side=half_side;
    BL.start = hc.start+half_side*hc.original_side;
    hillbert_curve_fill_D(BL,BL_start);
    

    TL=hc;
    TL.side  = half_side;
    hillbert_curve_fill_C(TL,TL_start);
    

    
}

static void hillbert_curve_fill_E(hilbert_curve_t hc,uint64_t start_index){
    if(hc.side==1){
        hc_at(hc,0,0) = start_index;
        return;
    }
    uint64_t half_side = hc.side/2;
    uint64_t quadrant_size =  half_side*half_side;
    
// E:
// → ↓
// • ←
// A -> E -> E -> F

    hilbert_curve_t TL,BL,BR,TR;
    uint64_t TL_start,BL_start,BR_start,TR_start;

    TL_start = start_index+0*quadrant_size;
    TR_start = start_index+1*quadrant_size;
    BR_start = start_index+2*quadrant_size;
    BL_start = start_index+3*quadrant_size;
    
    TL=hc;
    TL.side  = half_side;
    hillbert_curve_fill_A(TL,TL_start);
    

    TR=hc;
    TR.side  = half_side;
    TR.start = hc.start + half_side;
    hillbert_curve_fill_E(TR,TR_start);
    

    BR=hc;
    BR.side  = half_side;
    BR.start = hc.start+half_side*hc.original_side+half_side;
    hillbert_curve_fill_E(BR,BR_start);
    

    BL=hc;
    BL.side=half_side;
    BL.start = hc.start+half_side*hc.original_side;
    hillbert_curve_fill_F(BL,BL_start);
    
}

static void hillbert_curve_fill_F(hilbert_curve_t hc,uint64_t start_index){
    if(hc.side==1){
        hc_at(hc,0,0) = start_index;
        return;
    }
    uint64_t half_side = hc.side/2;
    uint64_t quadrant_size =  half_side*half_side;
    
// F:
// ↓ ←
// • ↑
// B -> F -> F -> E      

    hilbert_curve_t TL,BL,BR,TR;
    uint64_t TL_start,BL_start,BR_start,TR_start;

    BR_start = start_index+0*quadrant_size;
    TR_start = start_index+1*quadrant_size;
    TL_start = start_index+2*quadrant_size;
    BL_start = start_index+3*quadrant_size;
    
    BR=hc;
    BR.side  = half_side;
    BR.start = hc.start+half_side*hc.original_side+half_side;
    hillbert_curve_fill_B(BR,BR_start);
    

    TR=hc;
    TR.side  = half_side;
    TR.start = hc.start + half_side;
    hillbert_curve_fill_F(TR,TR_start);
    

    TL=hc;
    TL.side  = half_side;
    hillbert_curve_fill_F(TL,TL_start);
    

    BL=hc;
    BL.side=half_side;
    BL.start = hc.start+half_side*hc.original_side;
    hillbert_curve_fill_E(BL,BL_start);
    
}

static void hillbert_curve_fill_G(hilbert_curve_t hc,uint64_t start_index){
    if(hc.side==1){
        hc_at(hc,0,0) = start_index;
        return;
    }
    uint64_t half_side = hc.side/2;
    uint64_t quadrant_size =  half_side*half_side;
    
// G:
// → ↓
// ↑ •
// C -> G -> G -> H     

    hilbert_curve_t TL,BL,BR,TR;
    uint64_t TL_start,BL_start,BR_start,TR_start;

    BL_start = start_index+0*quadrant_size;
    TL_start = start_index+1*quadrant_size;
    TR_start = start_index+2*quadrant_size;
    BR_start = start_index+3*quadrant_size;
    
    BL=hc;
    BL.side=half_side;
    BL.start = hc.start+half_side*hc.original_side;
    hillbert_curve_fill_C(BL,BL_start);
    

    TL=hc;
    TL.side  = half_side;
    hillbert_curve_fill_G(TL,TL_start);
    

    TR=hc;
    TR.side  = half_side;
    TR.start = hc.start + half_side;
    hillbert_curve_fill_G(TR,TR_start);
    

    BR=hc;
    BR.side  = half_side;
    BR.start = hc.start+half_side*hc.original_side+half_side;
    hillbert_curve_fill_H(BR,BR_start);
    
}

static void hillbert_curve_fill_H(hilbert_curve_t hc,uint64_t start_index){
    if(hc.side==1){
        hc_at(hc,0,0) = start_index;
        return;
    }
    uint64_t half_side = hc.side/2;
    uint64_t quadrant_size =  half_side*half_side;
    
// H:
// ↓ ←
// → •  
// D -> H -> H -> G

    hilbert_curve_t TL,BL,BR,TR;
    uint64_t TL_start,BL_start,BR_start,TR_start;

    TR_start = start_index+0*quadrant_size;
    TL_start = start_index+1*quadrant_size;
    BL_start = start_index+2*quadrant_size;
    BR_start = start_index+3*quadrant_size;
    
    TR=hc;
    TR.side  = half_side;
    TR.start = hc.start + half_side;
    hillbert_curve_fill_D(TR,TR_start);
    

    TL=hc;
    TL.side  = half_side;
    hillbert_curve_fill_H(TL,TL_start);
    

    BL=hc;
    BL.side=half_side;
    BL.start = hc.start+half_side*hc.original_side;
    hillbert_curve_fill_H(BL,BL_start);
    

    BR=hc;
    BR.side  = half_side;
    BR.start = hc.start+half_side*hc.original_side+half_side;
    hillbert_curve_fill_G(BR,BR_start);
    
}


// build a hilbert curve of order n
static hilbert_curve_t hilbert_curve_of_order(uint8_t order){
    hilbert_curve_t hc;
    hc.original_side = 1<<order;
    hc.side = hc.original_side;
    hc.start=0;
    hc.data = (uint64_t*) calloc(hc.side*hc.side,sizeof(uint64_t));
    if (!hc.data) PERROR("Couldn't allocate space for hillbert curve of size %"PRIu64" x %"PRIu64"",hc.side,hc.side);

    hillbert_curve_fill_A(hc,0);
    

    return hc;
}

static maze_t get_maze_from_hilbert_curve(hilbert_curve_t hc){
    maze_t maze;
    alloc_maze(&maze,hc.side,hc.side);
    for(uint64_t y = 0 ; y < hc.side ; ++y){
        for(uint64_t x = 0 ; x < hc.side ; ++x){
            direction_t available = NORTH|EAST|SOUTH|WEST;

            // can't open to outside of the maze
            if(x==0) available &= ~WEST;
            if(x==hc.side-1) available &= ~EAST;
            if(y==0) available &= ~NORTH;
            if(y==hc.side-1) available &= ~SOUTH;

            // can't open to neighbors with lower hilbert index
            uint64_t this_hilbert_idx = hc_at(hc,x,y);
            if(available&WEST){
                uint64_t west_neighbor = hc_at(hc,x-1,y);
                if(west_neighbor<this_hilbert_idx)
                    available &= ~WEST;
            }
            if(available&NORTH){
                uint64_t north_neighbor = hc_at(hc,x,y-1);
                if(north_neighbor<this_hilbert_idx)
                    available &= ~NORTH;
            }
            if(available&EAST){
                uint64_t east_neighbor = hc_at(hc,x+1,y);
                if(east_neighbor<this_hilbert_idx)
                    available &= ~EAST;
            }
            if(available&SOUTH){
                uint64_t west_neighbor = hc_at(hc,x,y+1);
                if(west_neighbor<this_hilbert_idx)
                    available &= ~SOUTH;
            }
            if(available == 0 ) continue;
            direction_t randomized_direction =  random_direction(available);
            maze_at(maze,x,y).open_directions |= randomized_direction;
            switch (randomized_direction){
                case WEST:
                maze_at(maze,x-1,y).open_directions |= EAST;
                break;
                case NORTH:
                maze_at(maze,x,y-1).open_directions |= SOUTH;
                break;
                case EAST:
                maze_at(maze,x+1,y).open_directions |= WEST;
                break;
                case SOUTH:
                maze_at(maze,x,y+1).open_directions |= NORTH;
                break;

            default:
                break;
            }
            // printf("\n\nAt: %"PRIu64",%"PRIu64"\n",y,x);
            // printf("Available directions are: ");
            // print_direction(available);
            
        }
        // printf("\n");

    }
    return maze;
}

maze_t generate_random_maze_hillbert_lookahead(uint64_t side){
    uint8_t order = log2f(side);
    if(log2l(side)!= order)
        PERROR("Can only generate random mazes with hillbert lookahead with sides as powers of 2.");
    hilbert_curve_t hc = hilbert_curve_of_order(order);
    maze_t maze = get_maze_from_hilbert_curve(hc);
    return maze;
}

// test hilbert functions
void test_hilbert(){
    clock_t start = clock();
    hilbert_curve_t hc = hilbert_curve_of_order(13);
    // print_hilbert_curve(hc);
    maze_t maze = get_maze_from_hilbert_curve(hc);
    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    printf("finished after %.4fs...\n",seconds);
    print_maze(maze);
    free(hc.data);
}
