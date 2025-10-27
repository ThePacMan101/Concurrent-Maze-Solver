
bin/maze.o: maze.c
	gcc -o bin/maze.o -c maze.c -lm -pthread -Wall

bin/maze_mcmc.o: maze_mcmc.c
	gcc -o bin/maze_mcmc.o -c maze_mcmc.c -lm -pthread -Wall

bin/maze_hilbert.o: maze_hilbert.c
	gcc -o bin/maze_hilbert.o -c maze_hilbert.c -lm -pthread -Wall

bin/special_characters.o: special_characters.c
	gcc -o bin/special_characters.o -c special_characters.c -Wall

tests: tests.c bin/maze.o bin/maze_mcmc.o bin/maze_hilbert.o bin/special_characters.o
	gcc -o bin/tests tests.c bin/maze.o bin/maze_mcmc.o bin/maze_hilbert.o bin/special_characters.o -Wall -lm -pthread

clean:
	del /s /q bin