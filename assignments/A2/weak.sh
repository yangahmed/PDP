#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -p node -n 16
#SBATCH -t 3:00:00

echo "new"
module load gcc openmpi

echo "=============="

echo "=============="
echo "2 cores input 250000000 type 1"
mpirun -np 2 ./quicksort /proj/g2019005/nobackup/qsort_indata/input250000000.txt out.txt 1

echo "=============="
echo "2 cores input 250000000 type 2"
mpirun -np 2 ./quicksort /proj/g2019005/nobackup/qsort_indata/input250000000.txt out.txt 2

echo "=============="
echo "2 cores input 250000000 type 3"
mpirun -np 2 ./quicksort /proj/g2019005/nobackup/qsort_indata/input250000000.txt out.txt 3

echo "=============="
echo "4 cores input 500000000 type 1"
mpirun -np 4 ./quicksort /proj/g2019005/nobackup/qsort_indata/input500000000.txt out.txt 1

echo "=============="
echo "4 cores input 500000000 type 2"
mpirun -np 4 ./quicksort /proj/g2019005/nobackup/qsort_indata/input500000000.txt out.txt 2

echo "=============="
echo "4 cores input 500000000 type 3"
mpirun -np 4 ./quicksort /proj/g2019005/nobackup/qsort_indata/input500000000.txt out.txt 3

echo "=============="
echo "8 cores input 1000000000 type 1"
mpirun -np 8 ./quicksort /proj/g2019005/nobackup/qsort_indata/input1000000000.txt out.txt 1

echo "=============="
echo "8 cores input 1000000000 type 2"
mpirun -np 8 ./quicksort /proj/g2019005/nobackup/qsort_indata/input1000000000.txt out.txt 2

echo "=============="
echo "8 cores input 1000000000 type 3"
mpirun -np 8 ./quicksort /proj/g2019005/nobackup/qsort_indata/input1000000000.txt out.txt 3

echo "=============="
echo "16 cores input 2000000000 type 1"
mpirun -np 16 ./quicksort /proj/g2019005/nobackup/qsort_indata/input2000000000.txt out.txt 1

echo "=============="
echo "16 cores input 2000000000 type 2"
mpirun -np 16 ./quicksort /proj/g2019005/nobackup/qsort_indata/input2000000000.txt out.txt 2

echo "=============="
echo "16 cores input 2000000000 type 3"
mpirun -np 16 ./quicksort /proj/g2019005/nobackup/qsort_indata/input2000000000.txt out.txt 3