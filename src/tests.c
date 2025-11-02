#include "common.h"
#include "maze.h"
#include "solver.h"
#include <locale.h>
#include <wchar.h>

#define description_1 "generates a 50x50 maze"
void test_1() {
  uint64_t iterations = 50 * 50 * 50;
  maze_t maze = generate_random_maze_MCMC(50, 50, iterations);
  // print_maze(maze);
  uint32_t speed = 5000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_2 "generates NxN mazes, with N = {10 , 20, ..., 100}"
void test_2() {
  for (uint64_t size = 10; size <= 100; size += 10) {
    uint64_t iterations = size * size * size;
    maze_t maze = generate_random_maze_MCMC(size, size, iterations);
    // print_maze(maze);
    uint32_t speed = 5000;
    bool iterative_visualization = true;
    solve_maze(maze, CPU_CORES,iterative_visualization,speed);
    free(maze.data);
  }
}

#define description_3 "generates a 500x500 maze"
void test_3() {
  maze_t maze = generate_random_maze_MCMC(500, 500, 125000100);
  // print_maze(maze);
  uint32_t speed = 0;
  bool iterative_visualization = false;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_4 "generates as 32x32 maze in parallel (has loops, not a perfect maze)"
void test_4() {
  int_t size = 32;
  uint64_t iterations = size * size * size + 100;
  uint8_t power = log2l(CPU_CORES);
  uint8_t workers = 1 << power;
  maze_t maze = generate_random_maze_MCMC_parallel(size, size, iterations, workers);
  // print_maze(maze);
  uint32_t speed = 5000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_5 "generates a 10x10 maze shows and its slices"
void test_5() {
  int_t size = 10;
  uint64_t iterations = size * size * size;
  maze_t maze = generate_random_maze_MCMC(size, size, iterations);
  print_maze(maze);
  printf("----\n");
  maze_t sub_mazes[4];
  split_maze(maze, sub_mazes, 4);
  for (int i = 0; i < 4; ++i) {
    print_maze(sub_mazes[i]);
    printf("----\n");
  }
  free(maze.data);
}

#define description_6 "generates a 1024x1024 maze in parallel"
void test_6() {
  int_t size = 1024;
  uint64_t iterations = size * size * size + 100;
  uint8_t power = log2l(CPU_CORES);
  uint8_t workers = 1 << power;
  maze_t maze = generate_random_maze_MCMC_parallel(size, size, iterations, workers);
  // print_maze(maze);
  uint32_t speed = 0;
  bool iterative_visualization = false;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_7 "generates a 64 x 64 maze with hillbert lookahead"
void test_7() {
  clock_t start = clock();
  maze_t maze = generate_random_maze_hillbert_lookahead(64);
  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;
  printf("finished after %.4fs...\n", seconds);
  uint32_t speed = 5000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_8 "generates a 8.192 x 8.192 maze with hillbert lookahead"
void test_8() {
  clock_t start = clock();
  generate_random_maze_hillbert_lookahead(8192);
  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;
  printf("finished after %.4fs...\n", seconds);
}

#define description_9 "generates a 1024 x 1024 maze with hillbert lookahead"
void test_9() {
  clock_t start = clock();
  maze_t maze = generate_random_maze_hillbert_lookahead(1024);
  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;
  printf("finished after %.4fs...\n", seconds);
  uint32_t speed = 0;
  bool iterative_visualization = false;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_10 "generates a 256 x 256 maze with hillbert lookahead"
void test_10() {
  clock_t start = clock();
  maze_t maze = generate_random_maze_hillbert_lookahead(256);
  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;
  printf("finished after %.4fs...\n", seconds);
  // print_maze(maze);
  uint32_t speed = 500;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_11 "generates a 128 x 128 maze with hillbert lookahead"
void test_11() {
  clock_t start = clock();
  maze_t maze = generate_random_maze_hillbert_lookahead(128);
  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;
  printf("finished after %.4fs...\n", seconds);
  // print_maze(maze);
  uint32_t speed = 1000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_12 "generates a 32 x 32 maze with hillbert lookahead"
void test_12() {
  clock_t start = clock();
  maze_t maze = generate_random_maze_hillbert_lookahead(32);
  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;
  printf("finished after %.4fs...\n", seconds);
  // print_maze(maze);
  uint32_t speed = 5000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_13 "generates and solves a small 8x8 maze"
void test_13() {
  uint64_t iterations = 8 * 8 * 8;
  maze_t maze = generate_random_maze_MCMC(8, 8, iterations);
  // print_maze(maze);
  uint32_t speed = 300000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_14 "small test - 5x5 maze visualization"
void test_14() {
  uint64_t iterations = 5 * 5 * 5;
  maze_t maze = generate_random_maze_MCMC(5, 5, iterations);
  print_maze(maze);
  uint32_t speed = 0;
  bool iterative_visualization = false;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_15 "generates a straight 60x1 maze"
void test_15(){
  maze_t maze;
  alloc_maze(&maze,60,1);
  for(int i = 0 ; i < 60 ; ++i){
    maze_at(maze,i,0).open_directions|=(EAST|WEST);
  }
  maze_at(maze,0,0).open_directions = EAST;
  maze_at(maze,59,0).open_directions = WEST;
  // print_maze(maze);
  uint32_t speed = 5000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_16 "generates a straight 8000x1 maze"
void test_16(){
  maze_t maze;
  alloc_maze(&maze,8000,1);
  for(int i = 0 ; i < 8000 ; ++i){
    maze_at(maze,i,0).open_directions|=(EAST|WEST);
  }
  maze_at(maze,0,0).open_directions = EAST;
  maze_at(maze,7999,0).open_directions = WEST;
  // print_maze(maze);
  uint32_t speed = 2000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_17 "generates a simetric 32x32 maze with two ways from start to end"
void test_17(){
  maze_t maze;
  int_t side = 32;
  alloc_maze(&maze,side,side);
  for(int i = 0 ; i < side ; ++i){
    maze_at(maze,i,0).open_directions|=(EAST|WEST);
    maze_at(maze,0,i).open_directions|=(NORTH|SOUTH);
    maze_at(maze,i,side-1).open_directions|=(EAST|WEST);
    maze_at(maze,side-1,i).open_directions|=(NORTH|SOUTH);
  }
  maze_at(maze ,   0    ,   0    ).open_directions = ( EAST  | SOUTH );
  maze_at(maze ,   0    , side-1 ).open_directions = ( EAST  | NORTH );
  maze_at(maze , side-1 ,   0    ).open_directions = ( WEST  | SOUTH );
  maze_at(maze , side-1 , side-1 ).open_directions = ( WEST  | NORTH );
  // print_maze(maze);
  uint32_t speed = 10000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_18 "generates a simetric 32x32 maze with multiple ways from start to end"
void test_18(){
  maze_t maze;
  int_t side = 32;
  alloc_maze(&maze,side,side);
  for(int j = 0 ; j < side ; ++j){
    for(int i = 0 ; i < side ; ++i){
      maze_at(maze,i,j).open_directions|=(EAST|WEST|NORTH|SOUTH);
      if(i == 0     ) maze_at(maze,i,j).open_directions &= ~WEST;
      if(i == side-1) maze_at(maze,i,j).open_directions &= ~EAST;
      if(j == 0     ) maze_at(maze,i,j).open_directions &= ~NORTH;
      if(j == side-1) maze_at(maze,i,j).open_directions &= ~SOUTH;
    }
  }
  // print_maze(maze);
  uint32_t speed = 10000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

#define description_19 "generates a 32x32 maze, with two areas that can't be reached from each other"
void test_19(){
  int_t side = 32;
  maze_t maze = generate_random_maze_hillbert_lookahead(side);
  for(int i = 0 ; i < side ; ++i){
    maze_at(maze,15,i).open_directions = 0;
  }
  // print_maze(maze);
  uint32_t speed = 10000;
  bool iterative_visualization = true;
  solve_maze(maze, CPU_CORES,iterative_visualization,speed);
  free(maze.data);
}

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");
  srand(time(NULL));
  if (argc == 1) {
    printf("\n 1. ");
    printf(description_1);
    printf("\n 2. ");
    printf(description_2);
    printf("\n 3. ");
    printf(description_3);
    printf("\n 4. ");
    printf(description_4);
    printf("\n 5. ");
    printf(description_5);
    printf("\n 6. ");
    printf(description_6);
    printf("\n 7. ");
    printf(description_7);
    printf("\n 8. ");
    printf(description_8);
    printf("\n 9. ");
    printf(description_9);
    printf("\n10. ");
    printf(description_10);
    printf("\n11. ");
    printf(description_11);
    printf("\n12. ");
    printf(description_12);
    printf("\n13. ");
    printf(description_13);
    printf("\n14. ");
    printf(description_14);
    printf("\n15. ");
    printf(description_15);
    printf("\n16. ");
    printf(description_16);
    printf("\n17. ");
    printf(description_17);
    printf("\n18. ");
    printf(description_18);
    printf("\n19. ");
    printf(description_19);


    printf("\n\nexample:  ./bin/tests 1 5 6\n\n");
  }
  for (int_t i = 1; i < argc; ++i) {
    int_t test = atoi(argv[i]);
    switch (test) {
    case 1:
      test_1();
      break;
    case 2:
      test_2();
      break;
    case 3:
      test_3();
      break;
    case 4:
      test_4();
      break;
    case 5:
      test_5();
      break;
    case 6:
      test_6();
      break;
    case 7:
      test_7();
      break; // 7 takes too long, maybe it's broken?
    case 8:
      test_8();
      break;
    case 9:
      test_9();
      break;
    case 10:
      test_10();
      break;
    case 11:
      test_11();
      break;
    case 12:
      test_12();
      break;
    case 13:
      test_13();
      break;
    case 14:
      test_14();
      break;
    case 15:
      test_15();
      break;
    case 16:
      test_16();
      break;
    case 17:
      test_17();
      break;
    case 18:
      test_18();
      break;
    case 19:
      test_19();
      break;

    default:
      break;
    }
  }
}
