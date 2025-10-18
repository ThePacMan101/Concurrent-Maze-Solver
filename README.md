# Concurrent-Maze-Sover
___
## Project Description:

This project is a college assignment in which I create a concurrent application in C to solve a maze.

We also generate mazes using the Markov Chain Montecarlo method, which I discovered through [here](https://www.youtube.com/watch?v=zbXKcDVV4G0).

The goal is not to necessarly explore the maze faster, but to study the behaviour of our concurrent exploration algorithm, as explained later in this description.
___
## The Algorithm

Before we define the algorithm, first some things need to be stablished:
* The maze is a $M \times N$ matrix with each item being a struct that says what are the directions (North, East, South or West), one can move towards in the next step if they are on that item.

* We have a defined number of worker threads that explore the maze as needed (details bellow).

* We divide the matrix in smaller sections that are $m \times n$. Each one of those sections has a mutex variable so that we can exclude two or more worker threads from accessing the same data at the same time 

Now, to the definition:

First, we create all $W$ worker threads, and send $W-1$ of them to sleep. The remaining worker thread starts exploring the matrix in some vertex $(x_0,y_0)$.

___
## Examples:
Example of 50x25 maze generated using 15000 iterations of the MCMC method
![maze exemple 1](imgs/maze-1.png)