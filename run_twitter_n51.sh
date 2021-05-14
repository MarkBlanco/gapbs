#! /bin/bash

outfile="bfs_scaling_twitter.dat"
echo "$(date)" >> ${outfile} 
export OMP_PLACE="threads"
export OMP_NUM_THREADS=16
numactl -N 0 -m 0 ./bfs -f /sharedstorage/markb1/GAP_data/gap_format/twitter.sg -n 64 >> ${outfile} 2>&1 

export OMP_PLACE="cores"

export OMP_NUM_THREADS=8
numactl -N 0 -m 0 ./bfs -f /sharedstorage/markb1/GAP_data/gap_format/twitter.sg -n 64 >> ${outfile} 2>&1 

export OMP_NUM_THREADS=4
numactl -N 0 -m 0 ./bfs -f /sharedstorage/markb1/GAP_data/gap_format/twitter.sg -n 64 >> ${outfile} 2>&1 

export OMP_NUM_THREADS=2
numactl -N 0 -m 0 ./bfs -f /sharedstorage/markb1/GAP_data/gap_format/twitter.sg -n 64 >> ${outfile} 2>&1 

export OMP_NUM_THREADS=1
numactl -N 0 -m 0 ./bfs -f /sharedstorage/markb1/GAP_data/gap_format/twitter.sg -n 64 >> ${outfile} 2>&1 
