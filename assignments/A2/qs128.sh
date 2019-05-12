#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -p node -n 128
#SBATCH -t 1-00:00:00

echo "128 cores 125000000 nums"
module load gcc openmpi

echo "=============="
echo "input type 3"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 3