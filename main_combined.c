#include <stddef.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dispatch/dispatch.h>

typedef unsigned int elem_t;

#define ARRAY_SIZE 1000000      // liczba elementów
#define RUNS 100               // liczba powtórzeń
// #define PARALLEL_THRESHOLD 10000
#define SWAP(a, b) { elem_t tmp = (a); (a) = (b); (b) = tmp; }

static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static inline void generuj_liczby_losowe(elem_t *arr, size_t n) {
    for (size_t i = 0; i < n; i++)
        arr[i] = arc4random_uniform(sizeof(elem_t));
}

static inline int verify_sorted(const elem_t *arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            fprintf(stderr, "Błąd sortowania przy indeksie %zu: %lu > %lu\n",
                    i, arr[i - 1], arr[i]);
            return 0;
        }
    }
    return 1;
}

static inline elem_t median_of_three(elem_t *a, size_t x, size_t y, size_t z) {
    elem_t A = a[x], B = a[y], C = a[z];
    if ((A < B && B < C) || (C < B && B < A)) return B;
    else if ((B < A && A < C) || (C < A && A < B)) return A;
    else return C;
}

static inline void gs_bin_seq(elem_t *arr, long left, long right, int bit) {
    if (left >= right || bit < 0) return;

    elem_t mask = 1UL << bit;
    long i = left, j = right;

    while (i <= j) {
        while (i <= right && (arr[i] & mask) == 0) ++i;
        while (j >= left && (arr[j] & mask) != 0) --j;

        if (i < j) {
            SWAP(arr[i], arr[j]);
            ++i;
            if (0 > --j) break; // prevent underflow
        }
    }
    if (left < j) gs_bin_seq(arr, left, j, bit - 1);
    if (i < right) gs_bin_seq(arr, i, right, bit - 1);
}

// static inline void qs_bin_par(elem_t *arr, long left, long right, int bit,
//                        dispatch_queue_t queue, dispatch_group_t group) {
//     if (left >= right || bit < 0) return;
//     elem_t mask = 1UL << bit;
//     long i = left, j = right;
//     while (i <= j) {
//         while (i <= right && (arr[i] & mask) == 0) ++i;
//         while (j >= left && (arr[j] & mask) != 0) --j;
//         if (i < j) {
//             SWAP(arr[i], arr[j]);
//             i++;
//             if (j == 0) break; // prevent underflow
//             j--;
//         }
//     }
//     if ((right - left) > PARALLEL_THRESHOLD) {
//         dispatch_group_async(group, queue, ^{
//             qs_bin_par(arr, left, j, bit - 1, queue, group);
//         });
//         dispatch_group_async(group, queue, ^{
//             qs_bin_par(arr, i, right, bit - 1, queue, group);
//         });
//     } else {
//         gs_bin_seq(arr, left, j, bit - 1);
//         gs_bin_seq(arr, i, right, bit - 1);
//     }
// }

// static inline void qs_bin_parallel(elem_t *arr, size_t n) {
//     dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
//     dispatch_group_t group = dispatch_group_create();
//     qs_bin_par(arr, 0, n - 1, (int)(sizeof(elem_t)*8 - 1), queue, group);
//     dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
// }

static inline void qs_std_seq(elem_t *arr, long left, long right) {
    if (left >= right) return;
    elem_t pivot = median_of_three(arr, left, (left + right)/2, right);
    long i = left, j = right;
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
    if (left < j) qs_std_seq(arr, left, j);
    if (i < right) qs_std_seq(arr, i, right);
}

// static inline void qs_std_par(elem_t *arr, long left, long right,
//                    dispatch_queue_t queue, dispatch_group_t group) {
//     if (left >= right) return;
//     elem_t pivot = median_of_three(arr, left, (left + right)/2, right);
//     long i = left, j = right;
//     while (i <= j) {
//         while (arr[i] < pivot) i++;
//         while (arr[j] > pivot) j--;
//         if (i <= j) {
//             SWAP(arr[i], arr[j]);
//             i++;
//             if (j == 0) break; // prevent underflow
//             j--;
//         }
//     }
//     if ((right - left) > PARALLEL_THRESHOLD) {
//         dispatch_group_async(group, queue, ^{
//             qs_std_par(arr, left, j, queue, group);
//         });
//         dispatch_group_async(group, queue, ^{
//             qs_std_par(arr, i, right, queue, group);
//         });
//     } else {
//         if (left < j) qs_std_seq(arr, left, j);
//         if (i < right) qs_std_seq(arr, i, right);
//     }
// }

// static inline void qs_std_parallel(elem_t *arr, size_t n) {
//     dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
//     dispatch_group_t group = dispatch_group_create();
//     qs_std_par(arr, 0, n - 1, queue, group);
//     dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
// }


int main(void) {
    elem_t *arr_base    = malloc(ARRAY_SIZE * sizeof(elem_t));
    elem_t *arr_bit_seq = malloc(ARRAY_SIZE * sizeof(elem_t));
    // elem_t *arr_bit_par = malloc(ARRAY_SIZE * sizeof(elem_t));
    elem_t *arr_std_seq = malloc(ARRAY_SIZE * sizeof(elem_t));
    // elem_t *arr_std_par = malloc(ARRAY_SIZE * sizeof(elem_t));
    
    // if (!arr_base || !arr_bit_seq || !arr_bit_par || !arr_std_seq || !arr_std_par) {
    //     perror("malloc failed");
    //     return EXIT_FAILURE;
    // }

    if (!arr_base || !arr_bit_seq || !arr_std_seq) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }

    double total_bit_seq = 0.0;
    double total_std_seq = 0.0;
    double start, end;

    for (int r = 0; r < RUNS; r++) {
        generuj_liczby_losowe(arr_base, ARRAY_SIZE);

        // Przygotowanie czystych kopii
        for (size_t i = 0; i < ARRAY_SIZE; i++) {
            arr_bit_seq[i] = arr_base[i];
            arr_std_seq[i] = arr_base[i];
            // arr_bit_par[i] = arr_base[i];
            // arr_std_par[i] = arr_base[i];
        }

        // 1. Bitowy sekwencyjny
        start = now_sec();
        gs_bin_seq(arr_bit_seq, 0, ARRAY_SIZE - 1, (int)(sizeof(elem_t)*8 - 1));
        end = now_sec();
        total_bit_seq += (end - start);
        if (!verify_sorted(arr_bit_seq, ARRAY_SIZE)) { fprintf(stderr,"Błąd bit_seq\n"); break; }

        // 2. Bitowy równoległy
        // start = now_sec();
        // qs_bin_parallel(arr_bit_par, ARRAY_SIZE);
        // end = now_sec();
        // total_bit_par += (end - start);
        // if (!verify_sorted(arr_bit_par, ARRAY_SIZE)) { fprintf(stderr,"Błąd bit_par\n"); break; }

        // 3. Klasyczny sekwencyjny
        start = now_sec();
        qs_std_seq(arr_std_seq, 0, ARRAY_SIZE - 1);
        end = now_sec();
        total_std_seq += (end - start);
        if (!verify_sorted(arr_std_seq, ARRAY_SIZE)) { fprintf(stderr,"Błąd std_seq\n"); break; }

        // 4. Klasyczny równoległy
        // start = now_sec();
        // qs_std_parallel(arr_std_par, ARRAY_SIZE);
        // end = now_sec();
        // total_std_par += (end - start);
        // if (!verify_sorted(arr_std_par, ARRAY_SIZE)) { fprintf(stderr,"Błąd std_par\n"); break; }
    }

    printf("============================================================\n");
    printf("Benchmark sortowań (%d powtórzeń, %d elementów)\n", RUNS, ARRAY_SIZE);
    printf("------------------------------------------------------------\n");
    printf("Bitowy quicksort (1 rdzeń): total %.4fs | avg %.6fs\n", total_bit_seq, total_bit_seq / RUNS);
    // printf("Bitowy quicksort (multi):  total %.4fs | avg %.6fs\n", total_bit_par, total_bit_par / RUNS);
    printf("Klasyczny quicksort (1 rdzeń): total %.4fs | avg %.6fs\n", total_std_seq, total_std_seq / RUNS);
    // printf("Klasyczny quicksort (multi):  total %.4fs | avg %.6fs\n", total_std_par, total_std_par / RUNS);
    printf("------------------------------------------------------------\n");
    printf("Przyspieszenie sekwencyjne:  x%.2f\n", total_std_seq / total_bit_seq);
    // printf("Przyspieszenie równoległe: x%.2f\n", total_std_par / total_bit_par);
    printf("============================================================\n");

    free(arr_base);
    free(arr_bit_seq);
    // free(arr_bit_par);
    free(arr_std_seq);
    // free(arr_std_par);

    return 0;
}