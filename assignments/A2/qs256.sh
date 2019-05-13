#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -n 16
#SBATCH -t 01:00:00

echo "16 cores 125000000 nums"
module load gcc openmpi

echo "=============="
echo "input type 1"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 1

echo "=============="
echo "input type 2"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 2

echo "=============="
echo "input type 3"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 3

echo "=============="
echo "backwards type 1"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/backwards125000000.txt out.txt 1

echo "=============="
echo "backwards type 2"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/backwards125000000.txt out.txt 2

echo "=============="
echo "backwards type 3"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/backwards125000000.txt out.txt 3