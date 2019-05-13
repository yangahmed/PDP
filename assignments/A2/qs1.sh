#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -n 1
#SBATCH -t 30:00

echo "1 core"
module load gcc openmpi

echo "=============="
echo "input 125000000 type 3"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 3