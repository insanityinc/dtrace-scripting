SHELL = /bin/bash

BIN_NAME_S = sequential
BIN_NAME_G = germ

CC = gcc
FLAGS = -Wall -Wextra -g -Wno-unused-parameter -fopenmp -lm

all: sequential germ

sequential: sequentialTrace.c
	$(CC) -o $(BIN_NAME_S) sequential.c $(FLAGS)

germ: germ.c
	$(CC) -o $(BIN_NAME_G) germ.c $(FLAGS)

clean:
	rm $(BIN_NAME_S) $(BIN_NAME_G) data/*
