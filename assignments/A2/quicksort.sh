#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -t 10:00

module load gcc openmpi
mpirun alltoall
