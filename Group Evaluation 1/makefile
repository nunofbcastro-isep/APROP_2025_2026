BIN= init mmult mandel_seq mandel_par mandel

NUM_THREADS = 8

all: $(BIN)

init:
	mkdir -p bin
	
blocked_matrix_gcc: src/blocked_matrix.c
	gcc -Wall src/blocked_matrix.c -o bin/blocked_matrix -fopenmp -lm -fdump-ipa-cgraph

blocked_matrix_gcc_run: blocked_matrix_gcc
	bin/blocked_matrix $(NUM_THREADS)

blocked_matrix_llvm: src/blocked_matrix.c
	clang -Wall -fopenmp -lm src/blocked_matrix.c -o bin/blocked_matrix

blocked_matrix_llvm_run: blocked_matrix_llvm
	bin/blocked_matrix $(NUM_THREADS)