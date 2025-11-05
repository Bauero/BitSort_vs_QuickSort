#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Switch type if needed: unsigned int or unsigned long
typedef unsigned long elem_t;

#define SWAP(a, b) { elem_t tmp = (a); (a) = (b); (b) = tmp; }

static inline elem_t median_of_three(elem_t *arr, size_t a, size_t b, size_t c) {
    elem_t A = arr[a], B = arr[b], C = arr[c];
    if ((A < B && B < C) || (C < B && B < A)) return B;
    else if ((B < A && A < C) || (C < A && A < B)) return A;
    else return C;
}

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
            if (j == 0) break; // prevent underflow
            j--;
        }
    }

    // Recurse into smaller subarrays
    if (left < j) quicksort(arr, left, j);
    if (i < right) quicksort(arr, i, right);
}

int main(void) {
    const size_t N = 1000000;
    elem_t *arr = malloc(N * sizeof(elem_t));
    if (!arr) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < N; i++)
        arr[i] = arc4random_uniform(1000000000UL);

    quicksort(arr, 0, N - 1);

    // Verify sorted order
    for (size_t i = 1; i < N; i++) {
        if (arr[i - 1] > arr[i]) {
            fprintf(stderr, "Sort error at index %zu\n", i);
            free(arr);
            return EXIT_FAILURE;
        }
    }

    printf("Sorted %zu elements successfully.\n", N);
    free(arr);
    return EXIT_SUCCESS;
}