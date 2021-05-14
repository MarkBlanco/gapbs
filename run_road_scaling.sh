#! /bin/bash

PATH="/home/lonkar/gap/gapbs/benchmark/graphs/road.sg"
outfile="bfs_scaling.dat"
export OMP_PLACE="cores"
export OMP_NUM_THREADS=28
/usr/bin/numactl -N 0 -m 0 ./bfs -s -f ${PATH} -n 64 >> ${outfile} 2>&1 

export OMP_NUM_THREADS=16
/usr/bin/numactl -N 0 -m 0 ./bfs -s -f ${PATH} -n 64 >> ${outfile} 2>&1 

export OMP_NUM_THREADS=8
/usr/bin/numactl -N 0 -m 0 ./bfs -s -f ${PATH} -n 64 >> ${outfile} 2>&1 

export OMP_NUM_THREADS=4
/usr/bin/numactl -N 0 -m 0 ./bfs -s -f ${PATH} -n 64 >> ${outfile} 2>&1 

export OMP_NUM_THREADS=2
/usr/bin/numactl -N 0 -m 0 ./bfs -s -f ${PATH} -n 64 >> ${outfile} 2>&1 

export OMP_NUM_THREADS=1
/usr/bin/numactl -N 0 -m 0 ./bfs -s -f ${PATH} -n 64 >> ${outfile} 2>&1 
