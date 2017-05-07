#!/bin/bash
#SBATCH -J myMPI           # job name
#SBATCH -o myMPI.o%j       # output and error file name (%j expands to jobID)
#SBATCH -n 64              # total number of mpi tasks requested
#SBATCH -p development     # queue (partition) -- normal, development, etc.
#SBATCH -t 00:30:00        # run time (hh:mm:ss) - 1.5 hours
#SBATCH --mail-user=username@tacc.utexas.edu
#SBATCH --mail-type=begin  # email me when the job starts
#SBATCH --mail-type=end    # email me when the job finishes


#for((processor=2; processor <= 20; processor++))
#do
 #               echo " $processor"  >> ssort
  #  ibrun ./a.out $processor  >> ssort
#done


ibrun ./a.out 16777216 32  > tryMPI.txt        # run the MPI executable named a.out
