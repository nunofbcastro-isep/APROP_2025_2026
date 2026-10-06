#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void multiply(int **array1, int **array2, int **result, int rows1, int cols1, int cols2) {
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            result[i][j] = 0;
            for (int k = 0; k < cols1; k++) {
                result[i][j] += array1[i][k] * array2[k][j];
            }
        }
    }
}

int main() {
    int rows1 = 2, cols1 = 3, rows2 = 3, cols2 = 2;

    int **array1 = (int **)malloc(rows1 * sizeof(int *));
    for (int i = 0; i < rows1; i++) {
        array1[i] = (int *)malloc(cols1 * sizeof(int));
    }
    array1[0][0] = 1; array1[0][1] = 2; array1[0][2] = 3;
    array1[1][0] = 4; array1[1][1] = 5; array1[1][2] = 6;

    int **array2 = (int **)malloc(rows2 * sizeof(int *));
    for (int i = 0; i < rows2; i++) {
        array2[i] = (int *)malloc(cols2 * sizeof(int));
    }
    array2[0][0] = 7; array2[0][1] = 8;
    array2[1][0] = 9; array2[1][1] = 10;
    array2[2][0] = 11; array2[2][1] = 12;

    int **result = (int **)malloc(rows1 * sizeof(int *));
    for (int i = 0; i < rows1; i++) {
        result[i] = (int *)malloc(cols2 * sizeof(int));
    }

    clock_t start = clock();
    multiply(array1, array2, result, rows1, cols1, cols2);
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time for C: %f seconds\n", time_spent);

    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }

    // Free memory
    for (int i = 0; i < rows1; i++) free(array1[i]);
    free(array1);
    for (int i = 0; i < rows2; i++) free(array2[i]);
    free(array2);
    for (int i = 0; i < rows1; i++) free(result[i]);
    free(result);

    return 0;
}