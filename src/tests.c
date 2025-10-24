#include "maze.h"
#include "common.h"

void test_1(){
    uint64_t iterations = 50*50*50;
    maze_t maze = generate_random_maze(50,50,iterations);
    print_maze(maze);
    free(maze.data);
}

void test_2(){
    for(uint64_t size = 10 ; size <= 100 ; size+=10){
        uint64_t iterations = size*size*size;
        maze_t maze = generate_random_maze(size,size,iterations);
        print_maze(maze);
        free(maze.data);
    }
}

void test_3(){
    maze_t maze = generate_random_maze(500,500, 125000100);
    print_maze(maze);
    free(maze.data);
}

void test_4(){
    int_t size = 32;
    uint64_t iterations = size*size*size + 100;
    uint8_t power = log2l(CPU_CORES);
    uint8_t workers = 1<<power; 
    maze_t maze = generate_random_maze_parallel(size,size,iterations,workers);
    print_maze(maze);
    free(maze.data);
}

void test_5(){
    int_t size = 10;
    uint64_t iterations = size*size*size;
    maze_t maze = generate_random_maze(size,size,iterations);
    print_maze(maze);
    printf("----\n");
    maze_t sub_mazes[4];
    split_maze(maze,sub_mazes,4);
    for(int i = 0 ; i < 4 ; ++i){
        print_maze(sub_mazes[i]);
        printf("----\n");
    } 
    free(maze.data);
}

void test_6(){
    int_t size = 1024;
    uint64_t iterations = size*size*size + 100;
    uint8_t power = log2l(CPU_CORES);
    uint8_t workers = 1<<power; 
    maze_t maze = generate_random_maze_parallel(size,size,iterations,workers);
    print_maze(maze);
    free(maze.data);
}

int main(int argc, char ** argv){
    srand(42);
    
    for(int_t i = 1 ; i <= argc ;++i){
        int_t test = atoi(argv[i]);
        switch (test) {
            case 1: test_1(); break;
            case 2: test_2(); break;
            case 3: test_3(); break;
            case 4: test_4(); break;
            case 5: test_5(); break;
            case 6: test_6(); break;
            default: break;
        } 
    }
    
    
}


