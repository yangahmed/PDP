#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -p node -n 16
#SBATCH -t 10:00

echo "16 cores"
module load gcc openmpi

echo "=============="
echo "16 cores input 500000000 type 2"
mpirun -np 16 ./quicksort /proj/g2019005/nobackup/qsort_indata/input500000000.txt out.txt 2

