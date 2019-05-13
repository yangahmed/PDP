#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -n 16
#SBATCH -t 30:00

echo "16 cores"
module load gcc openmpi

echo "=============="
echo "input 2000000000 type 3"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input2000000000.txt out.txt 3

echo "=============="
echo "input 250000000 type 3"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input250000000.txt out.txt 3

echo "=============="
echo "input 250000000 type 2"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input250000000.txt out.txt 2

echo "=============="
echo "input 250000000 type 1"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input250000000.txt out.txt 1