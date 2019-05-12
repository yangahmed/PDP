#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -p node -n 512 -t 3:00:00

module load gcc openmpi
echo 512 cores
mpirun ./quicksort /proj/g2019005/nobackup/qsort_indata/input250000000.txt out.txt 1
