#!/bin/bash

mpicc -o $1 ./$1.c
mpirun -np 4 ./$1
