#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// You can switch between unsigned int / unsigned long here
typedef unsigned long elem_t;

#define SWAP(a, b) { elem_t tmp = (a); (a) = (b); (b) = tmp; }

// Median-of-three pivot choice
static inline elem_t median_of_three(elem_t *arr, size_t a, size_t b, size_t c) {
    elem_t A = arr[a], B = arr[b], C = arr[c];
    if ((A < B && B < C) || (C < B && B < A)) return B;
    else if ((B < A && A < C) || (C < A && A < B)) return A;
    else return C;
}

// Recursive quicksort
void quicksort(elem_t *arr, size_t left, size_t right) {
    if (left >= right) return;

    size_t i = left, j = right;
    elem_t pivot = median_of_three(arr, left, (left + right) / 2, right);

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            SWAP(arr[i], arr[j]);
            i++;
            if (j == 0) break; // avoid size_t underflow
            j--;
        }
    }

    if (left < j) quicksort(arr, left, j);
    if (i < right) quicksort(arr, i, right);
}

// Get current time in seconds (double)
static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(void) {
    const size_t N = 10000000;   // array size per test
    const int RUNS = 1000;      // number of repetitions
    elem_t *arr = malloc(N * sizeof(elem_t));
    if (!arr) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }

    double total_time = 0.0;

    for (int t = 0; t < RUNS; t++) {
        // Generate random data
        for (size_t i = 0; i < N; i++)
            arr[i] = arc4random_uniform(1000000000UL);

        // Measure sort time only
        double start = now_sec();
        quicksort(arr, 0, N - 1);
        double end = now_sec();
        total_time += (end - start);

        // Optional: sanity check for correct sort
        for (size_t i = 1; i < N; i++) {
            if (arr[i - 1] > arr[i]) {
                fprintf(stderr, "Sort error at iteration %d, index %zu\n", t, i);
                free(arr);
                return EXIT_FAILURE;
            }
        }
    }

    double avg_time = total_time / RUNS;
    printf("Sorted %zu elements %d times.\n", N, RUNS);
    printf("Total sorting time: %.6f seconds\n", total_time);
    printf("Average per sort:   %.6f seconds\n", avg_time);

    free(arr);
    return EXIT_SUCCESS;
}