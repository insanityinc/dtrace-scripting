SHELL = /bin/bash

BIN_NAME_P = omp
BIN_NAME_G = germ

CC = gcc
FLAGS = -Wall -Wextra -g -Wno-unused-parameter -fopenmp -lm

all: omp germ

omp: omp.c
	$(CC) -o $(BIN_NAME_P) omp.c $(FLAGS)

germ: germ.c
	$(CC) -o $(BIN_NAME_G) germ.c $(FLAGS)

clean:
	rm $(BIN_NAME_P) $(BIN_NAME_G) data/*
