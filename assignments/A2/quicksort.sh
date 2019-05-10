#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -t 10:00

module load gcc openmpi
mpirun ./quicksort.sh /proj/g2019005/nobackup/qsort_indata/backwards125000000.txt out.txt 1
