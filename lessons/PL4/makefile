BIN= init mmult mandel_seq mandel_par mandel

NUM_THREADS = 8

all: $(BIN)

init:
	mkdir -p bin
	
mmult: src/mmult.c
	gcc -Wall src/mmult.c -o bin/mmult -fopenmp 

mmult_run: mmult
	bin/mmult $(NUM_THREADS)

quicksort: src/quick_sort.c
	gcc -Wall src/quick_sort.c -o bin/quicksort -fopenmp 

quicksort_run: quicksort
	bin/quicksort $(NUM_THREADS)

sum_array: src/sum_array.c
	gcc -Wall src/sum_array.c -o bin/sum_array -fopenmp 

sum_array_run: sum_array
	bin/sum_array $(NUM_THREADS)

mandel: src/mandelbrot.c
	gcc -Wall src/mandelbrot.c -o bin/mandel -fopenmp -lm 

mandel_run: mandel
	bin/mandel $(NUM_THREADS)

blocked_matrix_gcc: src/blocked_matrix.c
	gcc -Wall src/blocked_matrix.c -o bin/blocked_matrix -fopenmp -lm -fdump-ipa-cgraph

blocked_matrix_gcc_run: blocked_matrix_gcc
	bin/blocked_matrix $(NUM_THREADS)

blocked_matrix_llvm: src/blocked_matrix.c
	clang -Wall -fopenmp -lm src/blocked_matrix.c -o bin/blocked_matrix

blocked_matrix_llvm_run: blocked_matrix_llvm
	bin/blocked_matrix $(NUM_THREADS)