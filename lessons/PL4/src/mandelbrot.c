/*
 * Copyright 2023 Instituto Superior de Engenharia do Porto
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
/*
**  PROGRAM: Mandelbrot area (solution)
**
**  PURPOSE: Program to compute the area of a  Mandelbrot set.
**           The correct answer should be around 1.510659.
**
**  USAGE:   Program runs without input ... just run the executable
**
**  ADDITIONAL EXERCISES:  Experiment with the schedule clause to fix 
**               the load imbalance.   Experiment with atomic vs. critical vs.
**               reduction for numoutside.
**            
**  HISTORY: Written:  (Mark Bull, August 2011).
**
**           Changed "comples" to "d_comples" to avoid collsion with 
**           math.h complex type.   Fixed data environment errors
**          (Tim Mattson, September 2011)
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define DEFAULT_NUM_THREADS 4

#define NPOINTS 1000
#define MAXITER 1000

typedef struct result_t{
   double area;
   double error;
}result_t;

struct d_complex{
   double r;
   double i;
};

struct d_complex c;
int numoutside = 0;

void testpoint(struct d_complex);
result_t seq_mandel();
result_t par_mandel_worksharing(int num_threads);
result_t par_mandel_tasks(int num_threads);

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int num_threads = DEFAULT_NUM_THREADS;
    if(argc < 2){
        printf("Number of threads was not specified. Will use default value: %d\n",DEFAULT_NUM_THREADS);
    }else{
        num_threads = atoi(argv[1]);
    }

    result_t expected;
    
    printf("Sequential Mandelbrot... ");
    clock_t begin = clock();
    expected = seq_mandel();
    clock_t end = clock();
    double seq_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("done.\n");
    printf("[SEQ]Area of Mandlebrot set = %12.8f +/- %12.8f (outside: %d)\n",expected.area,expected.error,numoutside);

    //resetting values
    int expected_num_outside = numoutside;
    numoutside = 0;
    
    printf("\nParallel Mandelbrot (Worksharing)... ");
    begin = clock();
    result_t par_worksharing_res = par_mandel_worksharing(num_threads);
    end = clock();
    double par_ws_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("done.\n");
    int par_ws_num_outside = numoutside;
    printf("[PAR Worksharing]Area of Mandlebrot set = %12.8f +/- %12.8f (outside: %d)\n",par_worksharing_res.area,par_worksharing_res.error,par_ws_num_outside);

    // simple check against sequential baseline
    if (expected.area != par_worksharing_res.area 
        || expected.error != par_worksharing_res.error
        || expected_num_outside != par_ws_num_outside){
        printf("[WARN] Worksharing result differs from sequential.\n");
    }

    // Reset and run the tasks-based version
    numoutside = 0;
    printf("\nParallel Mandelbrot (Tasks)... ");
    begin = clock();
    result_t par_tasks_res = par_mandel_tasks(num_threads);
    end = clock();
    double par_tasks_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("done.\n");
    int par_tasks_num_outside = numoutside;
    printf("[PAR Tasks]      Area of Mandlebrot set = %12.8f +/- %12.8f (outside: %d)\n",par_tasks_res.area,par_tasks_res.error,par_tasks_num_outside);

    if (expected.area != par_tasks_res.area 
        || expected.error != par_tasks_res.error
        || expected_num_outside != par_tasks_num_outside){
        printf("[WARN] Tasks result differs from sequential.\n");
    }

    printf("\n- ==== Performance ==== -\n");
    printf("Sequential time:          %fs\n",seq_time);
    printf("Parallel (Worksharing):   %fs\n",par_ws_time);
    printf("Parallel (Tasks):         %fs\n",par_tasks_time);
}

result_t par_mandel_worksharing(int num_threads){
    int i, j;
    double area, error, eps  = 1.0e-5;
    int outside_count = 0;

    omp_set_num_threads(num_threads);

    #pragma omp parallel for private(j) reduction(+:outside_count) schedule(dynamic)
    for (i=0; i<NPOINTS; i++) {
        for (j=0; j<NPOINTS; j++) {
            
            struct d_complex c_local, z;
            int iter;
            double temp;
            
            c_local.r = -2.0+2.5*(double)(i)/(double)(NPOINTS)+eps;
            c_local.i = 1.125*(double)(j)/(double)(NPOINTS)+eps;
            
            z=c_local;
            for (iter=0; iter<MAXITER; iter++){
                temp = (z.r*z.r)-(z.i*z.i)+c_local.r;
                z.i = z.r*z.i*2+c_local.i;
                z.r = temp;
                if ((z.r*z.r+z.i*z.i)>4.0) {
                    outside_count++;
                    break;
                }
            }
        }
    }
    
    numoutside = outside_count;

    area=2.0*2.5*1.125*(double)(NPOINTS*NPOINTS-numoutside)/(double)(NPOINTS*NPOINTS);
    error=area/(double)NPOINTS;

    result_t result = {area,error};
    return result;
}

result_t par_mandel_tasks(int num_threads){
    int i, j;
    double area, error, eps  = 1.0e-5;
    
    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        #pragma omp single
        {
            for (i=0; i<NPOINTS; i++) {
                #pragma omp task private(j)
                {
                    for (j=0; j<NPOINTS; j++) {
                        
                        struct d_complex c_local, z;
                        int iter;
                        double temp;
                        
                        c_local.r = -2.0+2.5*(double)(i)/(double)(NPOINTS)+eps;
                        c_local.i = 1.125*(double)(j)/(double)(NPOINTS)+eps;
                        
                        z=c_local;
                        for (iter=0; iter<MAXITER; iter++){
                            temp = (z.r*z.r)-(z.i*z.i)+c_local.r;
                            z.i = z.r*z.i*2+c_local.i;
                            z.r = temp;
                            if ((z.r*z.r+z.i*z.i)>4.0) {
                                #pragma omp atomic
                                numoutside++; 
                                break;
                            }
                        }
                    }
                }
            }
        }
        #pragma omp taskwait
    }
    
    area=2.0*2.5*1.125*(double)(NPOINTS*NPOINTS-numoutside)/(double)(NPOINTS*NPOINTS);
    error=area/(double)NPOINTS;

    result_t result = {area,error};
    return result;
}

result_t seq_mandel(){
    int i, j;
    double area, error, eps  = 1.0e-5;

    for (i=0; i<NPOINTS; i++) {
        for (j=0; j<NPOINTS; j++) {
            c.r = -2.0+2.5*(double)(i)/(double)(NPOINTS)+eps;
            c.i = 1.125*(double)(j)/(double)(NPOINTS)+eps;
            testpoint(c);
        }
    }

    // Calculate area of set and error estimate and output the results
    area=2.0*2.5*1.125*(double)(NPOINTS*NPOINTS-numoutside)/(double)(NPOINTS*NPOINTS);
    error=area/(double)NPOINTS;

    result_t result = {area,error};
    return result;
}

void testpoint(struct d_complex c){
    // Does the iteration z=z*z+c, until |z| > 2 when point is known to be outside set
    // If loop count reaches MAXITER, point is considered to be inside the set

    struct d_complex z;
    int iter;
    double temp;

    z=c;
    for (iter=0; iter<MAXITER; iter++){
        temp = (z.r*z.r)-(z.i*z.i)+c.r;
        z.i = z.r*z.i*2+c.i;
        z.r = temp;
        if ((z.r*z.r+z.i*z.i)>4.0) {
            numoutside++;
            break;
        }
    }

}