#include "maze.h"
#include "common.h"


char* description_1 = "generates a 50x50 maze";
void test_1(){
    uint64_t iterations = 50*50*50;
    maze_t maze = generate_random_maze_MCMC(50,50,iterations);
    print_maze(maze);
    free(maze.data);
}

char* description_2 = "generates NxN mazes, with N = {10 , 20, ..., 100}";
void test_2(){
    for(uint64_t size = 10 ; size <= 100 ; size+=10){
        uint64_t iterations = size*size*size;
        maze_t maze = generate_random_maze_MCMC(size,size,iterations);
        print_maze(maze);
        free(maze.data);
    }
}

char* description_3 = "generates a 500x500 maze";
void test_3(){
    maze_t maze = generate_random_maze_MCMC(500,500, 125000100);
    print_maze(maze);
    free(maze.data);
}

char* description_4 = "generates as 32x32 maze in parallel";
void test_4(){
    int_t size = 32;
    uint64_t iterations = size*size*size + 100;
    uint8_t power = log2l(CPU_CORES);
    uint8_t workers = 1<<power; 
    maze_t maze = generate_random_maze_MCMC_parallel(size,size,iterations,workers);
    print_maze(maze);
    free(maze.data);
}

char* description_5 = "generates a 10x10 maze shows and its slices";
void test_5(){
    int_t size = 10;
    uint64_t iterations = size*size*size;
    maze_t maze = generate_random_maze_MCMC(size,size,iterations);
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

char* description_6 =  "generates a 1024x1024 maze in parallel";
void test_6(){
    int_t size = 1024;
    uint64_t iterations = size*size*size + 100;
    uint8_t power = log2l(CPU_CORES);
    uint8_t workers = 1<<power; 
    maze_t maze = generate_random_maze_MCMC_parallel(size,size,iterations,workers);
    print_maze(maze);
    free(maze.data);
}

// takes to long (broken??)
char* description_7 = "generates a 4096x4096 maze in parallel";
void test_7(){
    int_t size = 4096;
    uint64_t iterations = ((uint64_t)size)*((uint64_t)size)*((uint64_t)size) + 100;
    uint8_t power = log2l(CPU_CORES);
    uint8_t workers = 1<<power; 
    maze_t maze = generate_random_maze_MCMC_parallel(size,size,iterations,workers);
    print_maze(maze);
    free(maze.data);
}

char* description_8 =  "generates a 8.192 x 8.192 hilbert curve fractal";
void test_8(){
    test_hilbert();
}

int main(int argc, char ** argv){
    srand(42);
    if(argc==1){
        printf("\n1. ");printf(description_1);
        printf("\n2. ");printf(description_2);
        printf("\n3. ");printf(description_3);
        printf("\n4. ");printf(description_4);
        printf("\n5. ");printf(description_5);
        printf("\n6. ");printf(description_6);
        printf("\n7. ");printf(description_7);

        printf("\n\nexample:  ./bin/tests 1 5 6\n\n");
    }
    for(int_t i = 1 ; i <= argc ;++i){
        int_t test = atoi(argv[i]);
        switch (test) {
            case 1: test_1(); break;
            case 2: test_2(); break;
            case 3: test_3(); break;
            case 4: test_4(); break;
            case 5: test_5(); break;
            case 6: test_6(); break;
            case 7: test_7(); break; // 7 takes too long, maybe it's broken?
            case 8: test_8();break;
            default: break;
        } 
    }
    
    
}


