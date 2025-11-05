#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <dispatch/dispatch.h>  // GCD for parallelism

typedef unsigned long elem_t;
#define SWAP(a, b) { elem_t tmp = (a); (a) = (b); (b) = tmp; }

// Threshold below which sorting runs sequentially
#define PARALLEL_THRESHOLD 50000

// Median-of-three pivot selection
static inline elem_t median_of_three(elem_t *arr, size_t a, size_t b, size_t c) {
    elem_t A = arr[a], B = arr[b], C = arr[c];
    if ((A < B && B < C) || (C < B && B < A)) return B;
    else if ((B < A && A < C) || (C < A && A < B)) return A;
    else return C;
}

// Recursive parallel quicksort
void quicksort_parallel(elem_t *arr, size_t left, size_t right, dispatch_queue_t queue, dispatch_group_t group) {
    if (left >= right) return;

    size_t i = left, j = right;
    elem_t pivot = median_of_three(arr, left, (left + right) / 2, right);

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            SWAP(arr[i], arr[j]);
            i++;
            if (j == 0) break;
            j--;
        }
    }

    // Parallelize only if range is large enough
    if ((right - left) > PARALLEL_THRESHOLD) {
        // Sort left side asynchronously
        dispatch_group_async(group, queue, ^{
            quicksort_parallel(arr, left, (j > 0 ? j : 0), queue, group);
        });
        // Sort right side asynchronously
        dispatch_group_async(group, queue, ^{
            quicksort_parallel(arr, i, right, queue, group);
        });
    } else {
        if (left < j) quicksort_parallel(arr, left, j, queue, group);
        if (i < right) quicksort_parallel(arr, i, right, queue, group);
    }
}

// Simple wrapper to make calling cleaner
void quicksort(elem_t *arr, size_t n) {
    dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    dispatch_group_t group = dispatch_group_create();

    quicksort_parallel(arr, 0, n - 1, queue, group);

    // Wait for all spawned tasks
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
}

// Time helper
static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(void) {
    const size_t N = 10000000;  // elements per run
    const int RUNS = 1000;
    elem_t *arr = malloc(N * sizeof(elem_t));
    if (!arr) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }

    double total_time = 0.0;

    for (int t = 0; t < RUNS; t++) {
        for (size_t i = 0; i < N; i++)
            arr[i] = arc4random_uniform(1000000000UL);

        double start = now_sec();
        quicksort(arr, N);
        double end = now_sec();
        total_time += (end - start);

        for (size_t i = 1; i < N; i++) {
            if (arr[i - 1] > arr[i]) {
                fprintf(stderr, "Sort error at iteration %d, index %zu\n", t, i);
                free(arr);
                return EXIT_FAILURE;
            }
        }
    }

    printf("Sorted %zu elements %d times (parallel quicksort).\n", N, RUNS);
    printf("Total sorting time: %.6f seconds\n", total_time);
    printf("Average per sort:   %.6f seconds\n", total_time / RUNS);

    free(arr);
    return EXIT_SUCCESS;
}