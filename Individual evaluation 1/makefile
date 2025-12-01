BIN= init clean build run

all: $(BIN)

init:
	mkdir -p bin

clean:
	rm -f bin/prime_2dvector

build: prime_2dvector.h seq.o 
	gcc -Wall prime_2dvector.c seq.o -o bin/prime_2dvector -fopenmp  -lm

run: build
	bin/prime_2dvector

