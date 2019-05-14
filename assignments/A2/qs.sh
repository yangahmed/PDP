#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -n 16
#SBATCH -t 30:00

echo "125000000 nums"
module load gcc openmpi

echo "=============="
echo "16 cores input type 1"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 1

echo "=============="
echo "4 cores input type 1"
mpirun -np 4 ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 1
