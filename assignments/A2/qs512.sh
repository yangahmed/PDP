#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -p node -n 512
#SBATCH -t 40:00

echo "512 cores"
module load gcc openmpi

echo "=============="
echo "input 125000000 type 3"
mpirun ./quicksort.sh /proj/g2019005/nobackup/qsort_indata/input125000000.txt out.txt 3

echo "=============="
echo "input 250000000 type 3"
mpirun ./quicksort.sh /proj/g2019005/nobackup/qsort_indata/input250000000.txt out.txt 3