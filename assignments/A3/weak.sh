#!/bin/bash -l

#SBATCH -A g2019005
#SBATCH -p node -n 25
#SBATCH -t 3:00:00

echo "weak"
module load gcc openmpi

echo "=============="
echo "4 core"
mpirun -np 4 ./matmul /proj/g2019005/nobackup/matmul_indata/input_5716.txt out

echo "=============="
echo "9 core"
mpirun -np 9 ./matmul /proj/g2019005/nobackup/matmul_indata/input_7488.txt out

echo "=============="
echo "16 core"
mpirun -np 16 ./matmul /proj/g2019005/nobackup/matmul_indata/input_9072.txt out

echo "=============="
echo "25 core"
mpirun -np 25 ./matmul /proj/g2019005/nobackup/matmul_indata/input_10525.txt out