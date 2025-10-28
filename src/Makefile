
bin/maze.o: maze.c
	gcc -o bin/maze.o -c maze.c -lm -pthread -Wall -O3

bin/maze_mcmc.o: maze_mcmc.c
	gcc -o bin/maze_mcmc.o -c maze_mcmc.c -lm -pthread -Wall -O3

bin/maze_hilbert.o: maze_hilbert.c
	gcc -o bin/maze_hilbert.o -c maze_hilbert.c -lm -pthread -Wall -O3

bin/special_characters.o: special_characters.c
	gcc -o bin/special_characters.o -c special_characters.c -Wall -O3

tests: tests.c bin/maze.o bin/maze_mcmc.o bin/maze_hilbert.o bin/special_characters.o
	gcc -o bin/tests tests.c bin/maze.o bin/maze_mcmc.o bin/maze_hilbert.o bin/special_characters.o -Wall -lm -pthread -O3

clean:
	del /s /q bin