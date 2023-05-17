#!/bin/bash
#SBATCH --job-name=haoli-mpi-p3
#SBATCH --account=amath
#SBATCH --partition=gpu-rtx6k
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=10
#SBATCH --mem=5G
#SBATCH --gpus=0
#SBATCH --time=00-00:10:00
#SBATCH --chdir=/gscratch/amath/haoli25/hw4
#SBATCH --export=all
#SBATCH --output=test-out.txt
#SBATCH --error=test-err.txt

module load gcc/11.2.0 ompi/4.1.3
mpic++ -o xmpi-p3 p3.cpp

for i in $(seq 1 40)
do 
    mpirun -np $i -allow-run-as-root ./xmpi-p3 100000000
done

mpirun -np 10 ./xmpi-p3 10
mpirun -np 10 ./xmpi-p3 100
mpirun -np 10 ./xmpi-p3 1000
mpirun -np 10 ./xmpi-p3 10000
mpirun -np 10 ./xmpi-p3 100000
mpirun -np 10 ./xmpi-p3 1000000

mpic++ -o xmpi-p4 p4.cpp
mpirun -np 40 ./xmpi-p4
