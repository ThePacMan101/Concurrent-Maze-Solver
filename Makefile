build:
	mkdir build -p

build/maze.o: src/maze.c build
	gcc -o build/maze.o -c src/maze.c -lm -pthread -Wall -O3 -Iinclude

build/maze_mcmc.o: src/maze_mcmc.c build
	gcc -o build/maze_mcmc.o -c src/maze_mcmc.c -lm -pthread -Wall -O3 -Iinclude

build/maze_hilbert.o: src/maze_hilbert.c build
	gcc -o build/maze_hilbert.o -c src/maze_hilbert.c -lm -pthread -Wall -O3 -Iinclude

build/special_characters.o: src/special_characters.c build
	gcc -o build/special_characters.o -c src/special_characters.c -Wall -O3 -Iinclude

build/solver_logic.o: src/solver.c build
	gcc -o build/solver_logic.o -c src/solver.c -lm -pthread -Wall -O3 -Iinclude

build/visualization.o: src/visualization.c build
	gcc -o build/visualization.o -c src/visualization.c -lm -pthread -Wall -O3 -Iinclude

build/main.o: src/main.c build
	gcc -o build/main.o -c src/main.c -lm -pthread -Wall -O3 -Iinclude

tests: src/tests.c build/visualization.o build/solver_logic.o build/maze.o build/maze_mcmc.o build/maze_hilbert.o build/special_characters.o build
	gcc -o build/tests src/tests.c build/visualization.o build/solver_logic.o build/maze.o build/maze_mcmc.o build/maze_hilbert.o build/special_characters.o -Wall -lm -pthread -O3 -Iinclude

solver: build/main.o build/solver_logic.o build/visualization.o build/maze.o build/maze_mcmc.o build/maze_hilbert.o build/special_characters.o build
	gcc -o build/solver build/main.o build/solver_logic.o build/visualization.o build/maze.o build/maze_mcmc.o build/maze_hilbert.o build/special_characters.o -Wall -lm -pthread -O3 -Iinclude

cleanw: build
	del /s /q build

cleanl: build
	rm build/**
