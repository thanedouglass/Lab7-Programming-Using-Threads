#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int MAX;

int **matA;
int **matB; 

int **matSumResult;
int **matDiffResult; 
int **matProductResult; 

void fillMatrix(int **matrix) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int **matrix) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Helper function to calculate row ranges robustly for any MAX and 10 threads
void get_row_range(int thread_id, int *start_row, int *end_row) {
    int rows_per_thread = MAX / 10;
    int remainder = MAX % 10;

    if (thread_id < remainder) {
        rows_per_thread++;
        *start_row = thread_id * rows_per_thread;
        *end_row = *start_row + rows_per_thread;
    } else {
        *start_row = thread_id * rows_per_thread + remainder;
        *end_row = *start_row + rows_per_thread;
    }
}

// Thread function for Matrix Addition
void* computeSum(void* args) {
    int thread_id = *((int*)args);
    free(args);

    int start_row, end_row;
    get_row_range(thread_id, &start_row, &end_row);

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }
    return NULL;
}

// Thread function for Matrix Subtraction
void* computeDiff(void* args) {
    int thread_id = *((int*)args);
    free(args);

    int start_row, end_row;
    get_row_range(thread_id, &start_row, &end_row);

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }
    return NULL;
}

// Thread function for Matrix Multiplication (Dot Product)
void* computeProduct(void* args) {
    int thread_id = *((int*)args);
    free(args);

    int start_row, end_row;
    get_row_range(thread_id, &start_row, &end_row);

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0;
            for (int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    
    // 0. Get the matrix size from the command line and assign it to MAX (default to 20)
    if (argc > 1) {
        MAX = atoi(argv[1]);
        if (MAX <= 0) {
            fprintf(stderr, "Error: Matrix size must be positive.\n");
            return 1;
        }
    } else {
        MAX = 20;
    }

    // Allocate memory for matrices with error checking
    matA = malloc(MAX * sizeof(int*));
    matB = malloc(MAX * sizeof(int*));
    matSumResult = malloc(MAX * sizeof(int*));
    matDiffResult = malloc(MAX * sizeof(int*));
    matProductResult = malloc(MAX * sizeof(int*));

    if (!matA || !matB || !matSumResult || !matDiffResult || !matProductResult) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    for(int i = 0; i < MAX; i++) {
        matA[i] = malloc(MAX * sizeof(int));
        matB[i] = malloc(MAX * sizeof(int));
        matSumResult[i] = malloc(MAX * sizeof(int));
        matDiffResult[i] = malloc(MAX * sizeof(int));
        matProductResult[i] = malloc(MAX * sizeof(int));

        if (!matA[i] || !matB[i] || !matSumResult[i] || !matDiffResult[i] || !matProductResult[i]) {
            fprintf(stderr, "Row memory allocation failed.\n");
            exit(1);
        }
    }
    
    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA);
    fillMatrix(matB);
    
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // 3. Create pthread_t objects for our 10 threads.
    pthread_t threads[10];
    
    // 4. Create threads for Matrix Addition
    for(int i = 0; i < 10; i++) {
        int* thread_id = malloc(sizeof(int));
        if (!thread_id) {
            fprintf(stderr, "Thread ID allocation failed.\n");
            exit(1);
        }
        *thread_id = i;
        if (pthread_create(&threads[i], NULL, computeSum, thread_id) != 0) {
            perror("pthread_create failed");
            exit(1);
        }
    }
    // 5. Wait for all addition threads to finish.
    for(int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    // Create threads for Matrix Subtraction
    for(int i = 0; i < 10; i++) {
        int* thread_id = malloc(sizeof(int));
        if (!thread_id) {
            fprintf(stderr, "Thread ID allocation failed.\n");
            exit(1);
        }
        *thread_id = i;
        if (pthread_create(&threads[i], NULL, computeDiff, thread_id) != 0) {
            perror("pthread_create failed");
            exit(1);
        }
    }
    // Wait for all subtraction threads to finish.
    for(int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    // Create threads for Matrix Multiplication (Dot Product)
    for(int i = 0; i < 10; i++) {
        int* thread_id = malloc(sizeof(int));
        if (!thread_id) {
            fprintf(stderr, "Thread ID allocation failed.\n");
            exit(1);
        }
        *thread_id = i;
        if (pthread_create(&threads[i], NULL, computeProduct, thread_id) != 0) {
            perror("pthread_create failed");
            exit(1);
        }
    }
    // Wait for all product threads to finish.
    for(int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);

    // Clean up allocated memory
    for(int i = 0; i < MAX; i++) {
        free(matA[i]);
        free(matB[i]);
        free(matSumResult[i]);
        free(matDiffResult[i]);
        free(matProductResult[i]);
    }
    free(matA);
    free(matB);
    free(matSumResult);
    free(matDiffResult);
    free(matProductResult);

    return 0;
}