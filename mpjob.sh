#!/bin/bash
#SBATCH --job-name="hellompi"
#SBATCH --output="hellompi.%j.%N.out"
#SBATCH --partition=compute
#SBATCH --nodes=16
#SBATCH --ntasks-per-node=16
#SBATCH --export=ALL
#SBATCH -t 04:00:00

#This job runs with 2 nodes, 24 cores per node for a total of 48 cores.
#ibrun in verbose mode will give binding detail

ibrun ./a.out 1024 > maxdist116.txt

