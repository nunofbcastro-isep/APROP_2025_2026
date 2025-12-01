#ifndef PRIME_2DVECTOR_H
#define PRIME_2DVECTOR_H

/**
 * This function will search the array for prime numbers and count how many are present, using a sequential algorithm.
 * @param array array to be searched
 * @param num_rows number of rows of the array
 * @param num_cols number of columns of the array
 * @return Returns the number of prime numbers available on the array.
 * */
int seq(int **array, int num_rows, int num_cols);

/**
 * This function is responsible to parse and setup the required variables for this quiz.
 * @param argc number of arguments
 * @param argv vector with arguments
 * @return Returns 1 if everything went okay, otherwise, -1 if arguments are being badly provided.
 * */
int setup(int argc, char* argv[]);

/**
 * This function is responsible to allocate and fill the 2d array with randomized numbers.
 * @param array Pointer to the array to be initialized and filled.
 * @return Returns 1 if everything went okay, otherwise, -2 if failed to allocate rows or -3 if faled to allocate collumns.
 * **/
int init_and_fill_array(int ***array);

/**
 * This function can be used to print the current array.
 * @param array Array to be printed
 * */
void print_array(int **array);

/**
 * This function is used to clean up and free the memory allocated on the application execution.
 * */
void clean_up(int **array);

#endif //PRIME_2DVECTOR_H
