/*
 * Copyright 2022 Instituto Superior de Engenharia do Porto
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

// Matrices dimensions, where A is LxM, B is MxN, and C is LxN
#define L 200
#define M 200
#define N 200

#define DEFAULT_USER_ROWS L/10

int A[L][M];
int B[M][N];
int C[L][N];
int expected[L][N];
double sequential_time;


#define MIN_RAND -10
#define MAX_RAND 10

//Matrix multiplication versions
/**
 * Matrix multiplication: A[L,M]* B[M,N] = C[L,N]
 **/
void seq();
void a_par_by_pos();
void b_par_by_row();
void c_par_by_user_rows(int num_lines_per_thread);

//Utility functions
void calc(int l,int n);
void fill(int* matrix, int height,int width);
void print(int* matrix,int height,int width);
void assert(int C[L][N],int expected[L][N]);
void c_clean();
void setup();

// C[0][0] = sum(A[0][i] * B[0][i]) for i = 0 to M ...
// do this from c[0][0] till c[L-1][N-1]
int main(int argc, char *argv[])
{
    srand(time(NULL));
    int user_block = DEFAULT_USER_ROWS;
    if(argc < 2){
        printf("Number of lines per thread was not specified, will use default value: %d\n", user_block);
    }else{
        user_block = atoi(argv[2]);
    }
    printf("Each thread processes %d rows to multiplicate two matrices: A{%d,%d}*B{%d,%d} = C{%d,%d}\n", user_block, L, M, M, N, L, N);

    setup();

    printf("Threads by Position...");
    clock_t begin = clock();
    a_par_by_pos();
    clock_t end = clock();
    double pos_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("done.\n");
    assert(C,expected);

    c_clean();
    
    printf("Thread per line...");
    begin = clock();
    b_par_by_row();
    end = clock();
    double per_row_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("done.\n");
    assert(C,expected);

    c_clean();

    printf("Thread processes set of lines (user-defined)...");
    begin = clock();
    c_par_by_user_rows(user_block); //user defined chunk
    end = clock();
    double block_of_rows_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("done.\n");
    assert(C,expected);

    printf("\n- ==== Performance ==== -\n");
    printf("Sequential time:          %fs\n",sequential_time);
    printf("Parallel Pos time:        %fs\n",pos_time);
    printf("Parallel per row time:    %fs\n",per_row_time);
    printf("Parallel rows block time: %fs\n",block_of_rows_time);  
}

/***
 * YOUR IMPLEMENTATIONS HERE! 
 **/
/**
 * Version where each thread is responsible for a single position of the result matrix
 **/

typedef struct {
    int row;
    int col;
} thread_a_data;

void* thread_a_calc(void* arg) {
    thread_a_data* data = (thread_a_data*)arg;
    calc(data->row, data->col);
    return NULL;
}

void a_par_by_pos(){
    pthread_t threads[L][N];
    thread_a_data thread_data[L][N];

    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            thread_data[l][n].row = l;
            thread_data[l][n].col = n;
            if (pthread_create(&threads[l][n], NULL, thread_a_calc, (void*)&thread_data[l][n]) != 0) {
                perror("Failed to create thread");
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            if (pthread_join(threads[l][n], NULL) != 0) {
                perror("Failed to join thread");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/**
 * Version where each thread is responsible for one line of the result matrix
 **/
 
void* thread_b_calc(void* arg) {
    int* row = (int*)arg;
    for (int l = 0; l < L; l++) {
        calc(l, *row);
    }
    return NULL;
}

void b_par_by_row(){
    pthread_t threads[N];
    int thread_data[N];

    for (int n = 0; n < N; n++)
    {
        thread_data[n] = nck;
        if (pthread_create(&threads[n], NULL, thread_b_calc, (void*)&thread_data[n]) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int n = 0; n < N; n++)
    {
        if (pthread_join(threads[n], NULL) != 0) {
            perror("Failed to join thread");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * Version where each thread is responsible for a number of user input lines 
 * of the result matrix
 **/
 

typedef struct {
    int first_row;
    int last_row;
} thread_c_data;

void* thread_c_calc(void* arg) {
    thread_c_data* data = (thread_c_data*)arg;
    for (int l = data->first_row; l <= data->last_row; l++) {
        for (int n = 0; n < N; n++) {
            calc(l, n);
        }
    }
    return NULL;
}


void c_par_by_user_rows(int num_lines_per_thread){
    int number_threads = L < num_lines_per_thread ? 1 : L / num_lines_per_thread;
    
    pthread_t threads[number_threads];
    thread_c_data thread_data[number_threads];

    for (int l = 0; l < number_threads; l++)
    {
        thread_data[l].first_row = l * num_lines_per_thread;
        thread_data[l].last_row = (l + 1) * num_lines_per_thread - 1;
        if (pthread_create(&threads[l], NULL, thread_c_calc, (void*)&thread_data[l]) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int l = 0; l < number_threads; l++)
    {
        if (pthread_join(threads[l], NULL) != 0) {
            perror("Failed to join thread");
            exit(EXIT_FAILURE);
        }
    }
}


/**

 * Example of a sequential matrix multiplication
*/
void seq()
{
    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            int sum = 0;
            for (int m = 0; m < M; m++)
            {
                sum += A[l][m] * B[m][n];
            }
            C[l][n] = sum;
        }
    }
}


///////////////////////////////////////////////////////////////////////////
/**
 * UTILITY FUNCTIONS
*/
void calc(int l,int n){
    int sum = 0;
    for (int m = 0; m < M; m++)
    {
        sum += A[l][m] * B[m][n];
    }
    C[l][n] = sum;
}

void fill(int* matrix, int height,int width){
    for (int l = 0; l < height; l++)
    {
        for (int n = 0; n < width; n++)
        {
            *((matrix+l*width) + n) = MIN_RAND + rand()%(MAX_RAND-MIN_RAND+1);
        }
    }
}

void print(int* matrix,int height,int width){
    
    for (int l = 0; l < height; l++)
    {
        printf("[");
        for (int n = 0; n < width; n++)
        {
            printf(" %5d",*((matrix+l*width) + n));
        }
        printf(" ]\n");
    }
}

void assert(int C[L][N],int expected[L][N]){
    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            if(C[l][n] != expected[l][n]){
                printf("Wrong value at position [%d,%d], expected %d, but got %d instead\n",l,n,expected[l][n],C[l][n]);
                exit(-1);
            }
        }
        
    }
}

void c_clean(){
    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            C[l][n] = 0;
        }
    }
}


void setup(){
    fill((int *)A,L,M);
    fill((int *)B,M,N);
    clock_t begin = clock();
    seq();
    clock_t end = clock();
    sequential_time = (double)(end - begin) / CLOCKS_PER_SEC;
    // print((int *)A,L,M);
    // printf("   *   \n");
    // print((int *)B,M,N);
    // printf("   =   \n");
    // print((int *)C,L,N);

    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            expected[l][n] = C[l][n];
            C[l][n] = 0;
        }
    }
}