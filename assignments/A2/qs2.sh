#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -n 2
#SBATCH -t 30:00

echo "2 cores"
module load gcc openmpi

echo "=============="
echo "input 500000000 type 2"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input500000000.txt out.txt 2