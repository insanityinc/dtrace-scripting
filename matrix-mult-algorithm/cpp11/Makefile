SHELL = /bin/bash

BIN_NAME = coo

CC = g++
FLAGS = -Wall -Wextra -g -Wno-unused-parameter -std=c++11 -lpthread

all: main

main: main.cpp coo.cpp
	$(CC) -o $(BIN_NAME) main.cpp coo.cpp $(FLAGS)

clean:
	rm $(BIN_NAME)
