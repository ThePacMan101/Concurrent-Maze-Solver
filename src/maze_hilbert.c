#include "common.h"
#include "maze.h"
#include "special_characters.h"


// Hilbert Curve:

// considering a 3x3 matrix as the following:
// 00 01 02    00 01 02    •  •  •
// 03 04 05 == 10 11 12 == •  •  •
// 06 07 08    20 21 22    •  •  •
//
// a hilbert curve is a matrix (actually an array interpreted
// as as matrix) with each cell being a pointer to another cell, 
// such as:
//
// order 1:
// ┌──┐    01 03    01 11  = → ↓
// │  │ == 00 03 == 00 11 == ↑ •  where • is the "origing" or "end"
//
// order 2:
// ┌──┐  ┌──┐    01 05 03 07    01 11 03 13    →  ↓  →  ↓
// │  └──┘  │    00 06 02 11    00 12 02 23    ↑  →  ↑  ↓
// └──┐  ┌──┘ == 04 08 14 10 == 10 20 32 22 == ↑  ←  ↓  ←
// ───┘  └───    13 09 15 15    31 21 33 33    →  ↑  →  •
// 
// Obs1: The side of a hillbert curve of order n is 2^n
// Obs2: • represents a pointer that points to itself

typedef struct{
    int_t *data;
    int_t side;
}hilbert_curve_t;
#define hc_at(hc,_x,_y) ((hc).data[(_x)+(_y)*(hc).side])
#define hc_pos(hc,_x,_y) ((_x)+(_y)*(hc).side)

static void print_hilbert_curve(hilbert_curve_t hc){
    for(int i = 0 ; i < hc.side ; ++i){
        for(int j = 0 ; j < hc.side ; ++j){
            if(i>0 && hc_at(hc,i,j) == hc_pos(hc,i-1,j)){
                print_spchar(L'←');
                print_spchar(L' ');
            }
            if(i<hc.side-1 && hc_at(hc,i,j) == hc_pos(hc,i+1,j)){
                print_spchar(L'→');
                print_spchar(L' ');
            }
            if(j>0 && hc_at(hc,i,j) == hc_pos(hc,i,j-1)){
                print_spchar(L'↑');
                print_spchar(L' ');
            }
            if(j<hc.side-1 && hc_at(hc,i,j) == hc_pos(hc,i,j+1)){
                print_spchar(L'↓');
                print_spchar(L' ');
            }
            if(hc_at(hc,i,j) == hc_pos(hc,i,j)){
                print_spchar(L'•');
                print_spchar(L' ');
            }
        }                
        print_spchar(L'\n');
    }
}


static hilbert_curve_t hilbert_curve_of_order(uint8_t order){
    hilbert_curve_t hc;
    hc.side = 2<<order;
    hc.data = (int_t*) calloc(hc.side,sizeof(int_t));

    if(order == 1){
        hc_at(hc,0,1) = hc_pos(hc,0,0);
        hc_at(hc,0,0) = hc_pos(hc,1,0);
        hc_at(hc,1,0) = hc_pos(hc,1,1);
        hc_at(hc,1,1) = hc_pos(hc,1,1);
    }



    return hc;
}

void test_hilbert(){
    hilbert_curve_t hc = hilbert_curve_of_order(1);
    print_hilbert_curve(hc);
}
