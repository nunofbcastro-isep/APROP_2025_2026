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
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <omp.h>

// Matrices dimensions, where A is LxM, B is MxN, and C is LxN
#define L 512*3
#define M 512*3
#define N 512*3

#define DEFAULT_NUM_THREADS 4

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
void par_row_collapse(int num_threads);
void par_row_collapse_1(int num_threads);
void par_row_collapse_2(int num_threads);
void par_row_scheduling_static(int num_threads);
void par_row_scheduling_dynamic(int num_threads);
void par_row_scheduling_guided(int num_threads);

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
    int num_threads = DEFAULT_NUM_THREADS;
    if(argc < 2){
        printf("Number of threads was not specified. Will use default value: %d\n",DEFAULT_NUM_THREADS);
    }else{
        num_threads = atoi(argv[1]);
    }
    printf("Working with %d threads to multiplicate two matrices: A{%d,%d}*B{%d,%d} = C{%d,%d}\n", num_threads, L, M, M, N, L, N);

    setup();
    
    // Test collapse
    printf("Testing collapse... ");
    double begin = omp_get_wtime();
    par_row_collapse(num_threads);
    double end = omp_get_wtime();
    double collapse_time = (end - begin);
    printf("done.\n");
    assert(C,expected);
    c_clean();

    // Test collapse(1)
    printf("Testing collapse(1)... ");
    begin = omp_get_wtime();
    par_row_collapse_1(num_threads);
    end = omp_get_wtime();
    double collapse_1_time = (end - begin);
    printf("done.\n");
    assert(C,expected);
    c_clean();

    // Test collapse(2)
    printf("Testing collapse(2)... ");
    begin = omp_get_wtime();
    par_row_collapse_2(num_threads);
    end = omp_get_wtime();
    double collapse_2_time = (end - begin);
    printf("done.\n");
    assert(C,expected);
    c_clean();

    // Test static scheduling
    printf("Testing static scheduling... ");
    begin = omp_get_wtime();
    par_row_scheduling_static(num_threads);
    end = omp_get_wtime();
    double static_time = (end - begin);
    printf("done.\n");
    assert(C,expected);
    c_clean();

    // Test dynamic scheduling
    printf("Testing dynamic scheduling... ");
    begin = omp_get_wtime();
    par_row_scheduling_dynamic(num_threads);
    end = omp_get_wtime();
    double dynamic_time = (end - begin);
    printf("done.\n");
    assert(C,expected);
    c_clean();

    // Test guided scheduling
    printf("Testing guided scheduling... ");
    begin = omp_get_wtime();
    par_row_scheduling_guided(num_threads);
    end = omp_get_wtime();
    double guided_time = (end - begin);
    printf("done.\n");
    assert(C,expected);

    printf("\n- ==== Performance Comparison ==== -\n");
    printf("Sequential time:        %fs\n", sequential_time);
    printf("Collapse(1) time:       %fs (speedup: %.2fx)\n", collapse_time, sequential_time/collapse_time);
    printf("Collapse(1) time:       %fs (speedup: %.2fx)\n", collapse_1_time, sequential_time/collapse_1_time);
    printf("Collapse(2) time:       %fs (speedup: %.2fx)\n", collapse_2_time, sequential_time/collapse_2_time);
    printf("Static scheduling:      %fs (speedup: %.2fx)\n", static_time, sequential_time/static_time);
    printf("Dynamic scheduling:     %fs (speedup: %.2fx)\n", dynamic_time, sequential_time/dynamic_time);
    printf("Guided scheduling:      %fs (speedup: %.2fx)\n", guided_time, sequential_time/guided_time);
}

/***
 * YOUR IMPLEMENTATIONS HERE! 
 **/
/**
 * Version where each thread is responsible for a set of rows
 **/
void par_row_collapse(int num_threads){
    #pragma omp parallel for num_threads(num_threads)
    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            calc(l,n);
        }
    }
}

void par_row_collapse_1(int num_threads){
    #pragma omp parallel for collapse(1) num_threads(num_threads)
    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            calc(l,n);
        }
    }
}

void par_row_collapse_2(int num_threads){
    #pragma omp parallel for collapse(2) num_threads(num_threads)
    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            calc(l,n);
        }
    }
}

void par_row_scheduling_static(int num_threads){
    #pragma omp parallel for num_threads(num_threads) schedule(static, 2)
    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            calc(l,n);
        }
    }
}

void par_row_scheduling_dynamic(int num_threads){
    #pragma omp parallel for num_threads(num_threads) schedule(dynamic, 2)
    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            calc(l,n);
        }
    }
}

void par_row_scheduling_guided(int num_threads){
    #pragma omp parallel for num_threads(num_threads) schedule(guided, 2)
    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            calc(l,n);
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

    for (int l = 0; l < L; l++)
    {
        for (int n = 0; n < N; n++)
        {
            expected[l][n] = C[l][n];
            C[l][n] = 0;
        }
    }
}