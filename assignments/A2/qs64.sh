#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -p node -n 64
#SBATCH -t 3-00:00:00

echo "64 cores"
module load gcc openmpi

echo "=============="
echo "input 125000000 type 3"
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 3