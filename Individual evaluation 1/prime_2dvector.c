//Nota adicionar o -lm ao gcc
//1240160

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <math.h>

#include "prime_2dvector.h"

#define DEFAULT_NUM_THREADS 4

//Define macros for default row and column size
#define DEFAULT_ROW_SIZE 10000
#define DEFAULT_COL_SIZE 10000

//Define maximum number to be random generated
#define MAX_RAND_NUM 10000

int num_threads = DEFAULT_NUM_THREADS;
int max_number = MAX_RAND_NUM;
int num_rows = DEFAULT_ROW_SIZE;
int num_cols = DEFAULT_COL_SIZE;

bool is_my_prime(int number){
    if (number <= 1)
    {
        return false;
    }
    
	if (number <= 3)
    {
        return true;
    }
    
	int number_sqrt = (int)sqrt((double)number);
	for (int i=2; i<=number_sqrt; i++) {
		if (number % i == 0 && i != number) {
			return false;
		}
	}
	
	return true;
}

int count_primes_in_vector(int **array, int num_rows, int num_cols)
{
	int count = 0;
	
	// 10000 x 10000
	// sequencial 1.94189150 s
	// so com for 0.78397420 s

	// schedule(dynamic) 0.75768701 s
	// schedule(dynamic,2) 0.73670069 s

	// schedule(guided) 0.77762282 s
	// schedule(guided,2) 0.76158134 s

	// schedule(static) 0.77602687 s
	// schedule(static,2) 0.75987756 s

	// collapse(1) 0.75504399 s
	// collapse(2) 0.78871691 s


    #pragma omp parallel for schedule(dynamic, 2) reduction(+:count) num_threads(num_threads)
	for(int i = 0; i < num_rows; i++){
		for(int j = 0; j < num_rows; j++){
			if(is_my_prime(array[i][j])){
				count ++;
			}
		}
	}
	return count;
}


int main(int argc, char* argv[])
{      
	int res = setup(argc,argv);
	if(!res)
	{
		return res;
	}

	int **array = NULL;

	res = init_and_fill_array(&array);
	if(!res)
	{
		return res;
	}
	
	int expected = 0, actual = 0;

	double start = omp_get_wtime();
	expected = seq(array,num_rows,num_cols);
	double end = omp_get_wtime();

	double seq_time = end-start;

	start = omp_get_wtime();
	actual = count_primes_in_vector(array,num_rows,num_cols);
	end = omp_get_wtime();
	double sol_time = end-start;
	
	if (expected != actual)
	{
		printf("ASSERT FAILED: Expected: %d - Actual %d\n", expected,actual);
		return 1;
	}

	printf("Prime count: %d\n", actual);
	printf("Sequential time: %.8f s\n", seq_time);
	printf("Parallel   time: %.8f s\n", sol_time); 

	clean_up(array);
	return 0;
}

// Auxiliar functions
int setup(int argc, char *argv[]) 
{
	if (argc > 1 && argc != 5) {
		fprintf(stderr, "Usage: %s NUM_THREADS MAX_NUMBER NUM_ROW_ARRAY NUM_COL_ARRAY\n", argv[0]);
		return -1;
    	} else if (argc == 5) {
		printf("Using command line arguments:\n");

		num_threads = atoi(argv[1]);
		max_number = atoi(argv[2]);
    		num_rows = atoi(argv[3]);
    		num_cols = atoi(argv[4]);
	} else 	{
		printf("Using default values:\n");
	}

    	printf("\tNumber of threads: %d\n", num_threads);
    	printf("\tMax number: %d\n", max_number);
    	printf("\tArray size: %d x %d\n", num_rows, num_cols);
	return 1;
}

int init_and_fill_array(int ***array)
{
	srand(time(NULL));
 	*array = malloc(num_rows * sizeof(int *));
    	if (*array == NULL) {
        	perror("Failed to allocate rows");
        	return -2;
    	}

    	for (int i = 0; i < num_rows; i++) {
        	(*array)[i] = malloc(num_cols * sizeof(int));
        	if ((*array)[i] == NULL) {
            		perror("Failed to allocate columns");
            		return -3;
        	}
	
        	for (int j = 0; j < num_cols; j++) {
            		(*array)[i][j] = rand() % max_number;
        	}
    	}
	
	return 1;
}

void print_array(int **array)
{
    	// Print array
    	printf("Print array:\n");
    	for (int i = 0; i < num_rows; i++) {
		printf("[ ");
        	for (int j = 0; j < num_cols; j++) {
            		printf("%d ", array[i][j]);
        	}
        	printf("]\n");
    	}
}

void clean_up(int **array)
{
    	// Free memory
    	for (int i = 0; i < num_rows; i++) {
        	free(array[i]);
    	}
    	free(array);
}
