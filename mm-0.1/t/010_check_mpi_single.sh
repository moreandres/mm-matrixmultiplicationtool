#! /bin/bash

mpirun -np 3 ../src/mm 256 mpi --check 2>&1 | grep -v Failed > /dev/null
