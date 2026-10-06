#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define DEFAULT_NUM_THREADS 4
#define ARRAY_SIZE 1000

void init_array(double *arr, int size);
double sequential_sum(double *arr, int size);
double manual_split_sum(double *arr, int size, int num_threads);
double reduction_for_sum(double *arr, int size, int num_threads);
double taskgroup_sum(double *arr, int size, int num_threads);
double taskloop_sum(double *arr, int size, int num_threads);

int main(int argc, char *argv[]) {
    int num_threads = DEFAULT_NUM_THREADS;
    if (argc >= 2) {
        num_threads = atoi(argv[1]);
    } else {
        printf("Number of threads was not specified. Will use default value: %d\n", DEFAULT_NUM_THREADS);
    }

    double *array = malloc(sizeof(double) * ARRAY_SIZE);
    if (!array) {
        fprintf(stderr, "Error allocating memory.\n");
        return 1;
    }

    init_array(array, ARRAY_SIZE);

    printf("Calculating sum of %d elements using %d threads...\n\n", ARRAY_SIZE, num_threads);

    // Sequential version for comparison
    double t_begin = omp_get_wtime();
    double sum_seq = sequential_sum(array, ARRAY_SIZE);
    double t_final = omp_get_wtime();
    double time_seq = t_final - t_begin;
    printf("[Sequential] Sum: %.2f (time: %.6fs)\n\n", sum_seq, time_seq);

    t_begin = omp_get_wtime();
    double sum1 = manual_split_sum(array, ARRAY_SIZE, num_threads);
    t_final = omp_get_wtime();
    double time1 = t_final - t_begin;
    printf("[a] Manual split + shared sum: %.2f (time: %.6fs)\n", sum1, time1);

    t_begin = omp_get_wtime();
    double sum2 = reduction_for_sum(array, ARRAY_SIZE, num_threads);
    t_final = omp_get_wtime();
    double time2 = t_final - t_begin;
    printf("[b] Parallel for with reduction: %.2f (time: %.6fs)\n", sum2, time2);

    t_begin = omp_get_wtime();
    double sum3 = taskgroup_sum(array, ARRAY_SIZE, num_threads);
    t_final = omp_get_wtime();
    double time3 = t_final - t_begin;
    printf("[c] Taskgroup with reduction: %.2f (time: %.6fs)\n", sum3, time3);

    t_begin = omp_get_wtime();
    double sum4 = taskloop_sum(array, ARRAY_SIZE, num_threads);
    t_final = omp_get_wtime();
    double time4 = t_final - t_begin;
    printf("[d] Taskloop with reduction: %.2f (time: %.6fs)\n", sum4, time4);

    printf("\n===== Performance Summary =====\n");
    printf("Sequential:    %.6fs (baseline)\n", time_seq);
    printf("Manual split:  %.6fs (speedup: %.2fx)\n", time1, time_seq/time1);
    printf("For reduction: %.6fs (speedup: %.2fx)\n", time2, time_seq/time2);
    printf("Taskgroup:     %.6fs (speedup: %.2fx)\n", time3, time_seq/time3);
    printf("Taskloop:      %.6fs (speedup: %.2fx)\n", time4, time_seq/time4);

    free(array);
    return 0;
}

void init_array(double *arr, int size) {
    srand(42);
    for (int i = 0; i < size; i++) {
        arr[i] = (double)(rand() % 1000) / 10.0;
    }
}

/* Sequential version for baseline comparison */
double sequential_sum(double *arr, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

/* (a) Explicit handling of the array splitting and shared sum variable */
double manual_split_sum(double *arr, int size, int num_threads) {
    double sum = 0.0;

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        int chunk = size / num_threads;
        int start = tid * chunk;
        int end = (tid == num_threads - 1) ? size : start + chunk;

        double local_sum = 0.0;
        for (int i = start; i < end; i++) {
            local_sum += arr[i];
        }

        #pragma omp atomic
        sum += local_sum;
    }

    return sum;
}

/* (b) Parallel for with reduction */
double reduction_for_sum(double *arr, int size, int num_threads) {
    double sum = 0.0;

    #pragma omp parallel for num_threads(num_threads) reduction(+:sum)
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }

    return sum;
}

/* (c) Parallel taskgroup with reduction */
double taskgroup_sum(double *arr, int size, int num_threads) {
    double sum = 0.0;

    #pragma omp parallel num_threads(num_threads)
    #pragma omp single
    {
        #pragma omp taskgroup task_reduction(+: sum)
        {
            for (int i = 0; i < size; i++) {
                #pragma omp task in_reduction(+: sum)
                {
                    sum += arr[i];
                }
            }
        }
    }

    return sum;
}

/* (d) Parallel taskloop with reduction */
double taskloop_sum(double *arr, int size, int num_threads) {
    double sum = 0.0;

    #pragma omp parallel num_threads(num_threads)
    {
        #pragma omp single
        {
            #pragma omp taskloop reduction(+:sum)
            for (int i = 0; i < size; i++) {
                sum += arr[i];
            }
        }
    }

    return sum;
}