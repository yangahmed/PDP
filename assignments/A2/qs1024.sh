#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -p node -n 1024
#SBATCH -t 30:00

echo "1024 cores"
module load gcc openmpi

echo "=============="
echo "input 125000000 type 3"
mpirun ./quicksort.sh /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 3

echo "=============="
echo "input 500000000 type 3"
mpirun ./quicksort.sh /proj/g2019005/nobackup/qsort_indata/input500000000.txt out.txt 3