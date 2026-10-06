#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define DEFAULT_NUM_THREADS 4
#define N 512        // matrix size
#define BS 64        // block size (assume N % BS == 0)
#define DEFAULT_RUNS 20

void init_matrix(double M[N][N]);
void print_matrix(double M[N][N], int max_display);
void seq_process(double M[N][N]);
void par_tasks_element(double M[N][N], int num_threads);
void par_tasks_block(double M[N][N], int num_threads);
int assert_matrix_equal(double A[N][N], double B[N][N], double eps);

// Generic process function type that accepts a matrix and a thread count
typedef void (*process_fn_t)(double M[N][N], int num_threads);

// Helper to execute a function multiple times, validate against sequential reference, and average the timings
double run_and_average(process_fn_t fn, int num_threads, int runs, double eps, const char *label);

// Simple wrapper to adapt seq_process to the (M, threads) function signature
void seq_wrapper(double M[N][N], int num_threads);

int main(int argc, char *argv[]) {
    int num_threads = DEFAULT_NUM_THREADS;
    if (argc >= 2)
        num_threads = atoi(argv[1]);
    else
        printf("Number of threads not specified. Using default: %d\n", DEFAULT_NUM_THREADS);

    int runs = DEFAULT_RUNS;
    if (argc >= 3)
        runs = atoi(argv[2]);
    else
        printf("Runs not specified. Using default: %d\n", DEFAULT_RUNS);

    printf("Matrix size: %dx%d | Block size: %d | Threads: %d | Runs: %d\n\n", N, N, BS, num_threads, runs);

    double avg_seq = run_and_average(seq_wrapper, num_threads, runs, 1e-9, "SEQ");
    if (avg_seq < 0) return 1;
    printf("[SEQ] Average time over %d runs: %.6fs\n", runs, avg_seq);

    double avg_elem = run_and_average(par_tasks_element, num_threads, runs, 1e-9, "PAR - element tasks");
    if (avg_elem < 0) return 1;
    printf("[PAR - element tasks] Average time over %d runs: %.6fs\n", runs, avg_elem);

    double avg_block = run_and_average(par_tasks_block, num_threads, runs, 1e-9, "PAR - block tasks");
    if (avg_block < 0) return 1;
    printf("[PAR - block tasks]   Average time over %d runs: %.6fs\n", runs, avg_block);

    return 0;
}

/* Initialise matrix with simple values */
void init_matrix(double M[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            M[i][j] = (i + j) % 100;
}

/* Simple print for debugging */
void print_matrix(double M[N][N], int max_display) {
    for (int i = 0; i < max_display; i++) {
        for (int j = 0; j < max_display; j++)
            printf("%6.1f ", M[i][j]);
        printf("\n");
    }
}

/* Sequential reference version */
void seq_process(double M[N][N]) {
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            M[i][j] = (M[i][j - 1] + M[i - 1][j] + M[i][j + 1] + M[i + 1][j]) / 4.0;
        }
    }
}

/* (a) Parallelize the following code, with tasks and dependencies */
void par_tasks_element(double M[N][N], int num_threads) {
    #pragma omp parallel num_threads(num_threads)
    {
        #pragma omp single
        {
            for (int i = 1; i < N - 1; i++) {
                for (int j = 1; j < N - 1; j++) {
                    #pragma omp task depend(in: M[i][j-1], M[i-1][j], M[i][j+1], M[i+1][j]) depend(out: M[i][j])
                    {
                        M[i][j] = (M[i][j - 1] + M[i - 1][j] + M[i][j + 1] + M[i + 1][j]) / 4.0;
                    }
                }
            }
        }
    }
}

/* (b) Change to divide the matrix in blocks of BS size. Inside each block the execution is sequential (assume N is multiple of BS). */
void par_tasks_block(double M[N][N], int num_threads) {
    #pragma omp parallel num_threads(num_threads)
    {
        #pragma omp single
        {
            for (int ii = 0; ii < BS; ii++) {
                for (int jj = 0; jj < BS; jj++) {
                    
                    int inf_i = 1 + ii * BS;
                    int inf_j = 1 + jj * BS;

                    int sup_i = (inf_i + BS < N - 1) ? inf_i + BS : N - 1;
                    int sup_j = (inf_j + BS < N - 1) ? inf_j + BS : N - 1;

                    #pragma omp task depend(in: M[inf_i-BS][inf_j: sup_j-1], \
                                               M[inf_i: sup_i-1][inf_j-BS]) \
                                     depend(inout: M[inf_i: sup_i-1][inf_j: sup_j-1])
                    {
                        for(int i = inf_i; i < sup_i; i++){
                            for(int j = inf_j; j < sup_j; j++){
                               M[i][j] = (M[i][j - 1] + M[i - 1][j] + M[i][j + 1] + M[i + 1][j]) / 4.0;
                            }
                        }
                    }
                }
            }
        }
    }
}

int assert_matrix_equal(double A[N][N], double B[N][N], double eps) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double diff = fabs(A[i][j] - B[i][j]);
            if (diff > eps) {
                printf("[ERROR] Mismatch at (%d,%d): expected %.10f, got %.10f (|diff|=%.3e)\n",
                       i, j, A[i][j], B[i][j], diff);
                return 0;
            }
        }
    }
    return 1;
}

// Wrapper to adapt sequential function to (M, threads) signature
void seq_wrapper(double M[N][N], int num_threads) {
    (void)num_threads; // unused
    seq_process(M);
}

double run_and_average(process_fn_t fn, int num_threads, int runs, double eps, const char *label) {
    if (runs <= 0) {
        fprintf(stderr, "[ERROR] runs must be > 0\n");
        return -1.0;
    }

    static double M_ref[N][N];
    static double M_work[N][N];

    // Build reference once from the canonical initial matrix
    init_matrix(M_ref);
    seq_process(M_ref);

    double total = 0.0;
    for (int r = 0; r < runs; r++) {
        init_matrix(M_work);
        double t0 = omp_get_wtime();
        fn(M_work, num_threads);
        double t1 = omp_get_wtime();
        total += (t1 - t0);

        if (!assert_matrix_equal(M_ref, M_work, eps)) {
            fprintf(stderr, "[CHECK] Validation failed in run %d for %s.\n", r + 1, label ? label : "(no label)");
            return -1.0;
        }
    }

    printf("[%s] All %d runs validated successfully.\n", label ? label : "RUN", runs);
    return total / runs;
}