SHELL = /bin/sh

BIN_SPMV = spmv_v2
BIN_GEN  = gen

SRC_SPMV = spmv_v2.c
SRC_GEN  = gen.c

SRC_DIR = src
BIN_DIR = bin

CC = mpicc

FLAGS = -O3 -Wall -Wno-unused-result -std=c99 -lm

### RULES ###

all: spmv gen

spmv: $(SRC_DIR)/$(SRC_SPMV)
	$(CC) -o $(BIN_DIR)/$(BIN_SPMV) $(SRC_DIR)/$(SRC_SPMV) $(FLAGS)

gen: $(SRC_DIR)/$(SRC_GEN)
	$(CC) -o $(BIN_DIR)/$(BIN_GEN) $(SRC_DIR)/$(SRC_GEN) $(FLAGS)

clean:
	rm -f $(BIN_DIR)/* data/*
