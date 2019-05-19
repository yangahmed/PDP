#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -p node -n 16
#SBATCH -t 30:00

echo "16 cores"
module load gcc openmpi

echo "=============="
echo "backwards 2000000000 type 1"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/backwards2000000000.txt out.txt 1

echo "=============="
echo "backwards 2000000000 type 2"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/backwards2000000000.txt out.txt 2

echo "=============="
echo "backwards 2000000000 type 3"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/backwards2000000000.txt out.txt 3
