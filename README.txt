ffAMATH 583: High Performance Scientific Computing
This the homework 4 for hpc course at the University of Washington. 

• All plots have corresponding name for each problem. 

• For p1, it's just regular C++ compile.

• For p2, after compile, it takes two arguments to run the program: number of partitions and number
of threads. 

	./p2 1000000 6

	1000000 partitions with 6 threads.

• For p5, after compile, it take one arguments to run the program: number of people are going to serve.

	./p5_2 20

	20 people are going to take the elevator.

• For p3 and p4, we use .slurm file to submit job on Hyak. All the instructions are written in haoli-mpi.slurm file.