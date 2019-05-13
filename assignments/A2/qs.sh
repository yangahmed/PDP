#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -p node -n 16
#SBATCH -t 3-00:00:00

echo "125000000 nums"
module load gcc openmpi

echo "=============="
echo "8 cores input type 3"
mpirun -np 8 ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 3

echo "=============="
echo "4 cores input type 3"
mpirun -np 4 ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 3
