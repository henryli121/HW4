# AMATH 583: High Performance Scientific Computing

## Homework 4

This repository contains the fourth homework assignment for the High Performance Scientific Computing (HPC) course at the University of Washington.

## Content

- All plots have corresponding names for each problem.

### Problem 1

- For problem 1, it's just a regular C++ compile.

### Problem 2

- For problem 2, after compile, it requires two arguments to run the program: number of partitions and number of threads. 

```bash
./p2 1000000 6 
```
The command above instructs the program to create 1,000,000 partitions, utilizing 6 threads to do so.

### Problem 5

For problem 5, the compiled program requires a single argument to run: the number of people who are to be served.

```bash
./p5_2 20
```
Executing the above command suggests that 20 people will use the elevator service.

### Problem 3 and Problem 4

For problems 3 and 4, we leverage a `.slurm` file to orchestrate job submissions on the Hyak supercomputer. You'll find all the necessary instructions and details within the `haoli-mpi.slurm` file. Please ensure to read and understand them before initiating any job submissions.

