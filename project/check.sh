#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -t 1:00:00

mpirun -np 1 ./matmul /proj/g2019005/nobackup/matmul_indata/input3600.txt out