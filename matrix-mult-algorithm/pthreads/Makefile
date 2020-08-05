SHELL = /bin/bash

BIN_NAME_P = pthreads
BIN_NAME_G = germ

CC = gcc
FLAGS = -Wall -Wextra -g -Wno-unused-parameter -lpthread -lm

all: pthreads germ

pthreads: pthreads.c
	$(CC) -o $(BIN_NAME_P) pthreads.c $(FLAGS)

germ: germ.c
	$(CC) -o $(BIN_NAME_G) germ.c $(FLAGS)

clean:
	rm $(BIN_NAME_P) $(BIN_NAME_G) data/*
