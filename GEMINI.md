# GEMINI.md

## Project Overview

This project is a concurrent maze solver written in C. It implements algorithms for generating mazes using the Markov Chain Monte Carlo (MCMC) and Hilbert Lookahead methods. The primary goal of the project is to study the behavior of a concurrent maze exploration algorithm. The project uses pthreads for concurrency.

The maze itself is represented as a 2D grid, and the solver uses multiple worker threads to explore the maze concurrently. When a worker thread finds a junction with multiple unexplored paths, it attempts to wake up sleeping threads to explore the other paths.

## Building and Running

### Building the Project

To build the project, navigate to the `src` directory and run the `make` command:

```sh
cd src
make tests
```

This will compile the source code and create an executable file at `bin/tests`.

### Running Tests

To run the tests, execute the `bin/tests` executable from the `src` directory. You can specify which tests to run by providing their numbers as command-line arguments.

To see a list of available tests and their descriptions, run the executable without any arguments:

```sh
./bin/tests
```

To run specific tests (e.g., tests 1, 2, and 4):

```sh
./bin/tests 1 2 4
```

## Development Conventions

*   **Language:** The project is written in C.
*   **Concurrency:** Concurrency is implemented using the `pthread` library.
*   **Compilation:** The project is compiled with `gcc` using the `-Wall -lm -pthread -O3` flags.
*   **Code Style:** The code is organized into modules with header files (`.h`) for interfaces and source files (`.c`) for implementations.
*   **Error Handling:** A custom `PERROR` macro is defined in `src/common.h` for standardized error reporting.
*   **Solver Status:** The core solver logic in `src/solver.c` is not yet implemented. The existing code focuses on maze generation and testing infrastructure.
