#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <dispatch/dispatch.h>

typedef unsigned long elem_t;

// Konfiguracja
#define ARRAY_SIZE 1000000
#define RUNS 20
#define PARALLEL_THRESHOLD 100000  // próg do uruchomienia w wielu wątkach

// --- Pomiar czasu ---
static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// --- Generator danych ---
void generuj_liczby_losowe(elem_t *array, size_t size) {
    for (size_t i = 0; i < size; i++)
        array[i] = arc4random();
}

// ======================================================
//   WERSJA SEKWENCYJNA (1 wątek)
// ======================================================

void bit_quicksort_seq(elem_t *array, long left, long right, int bit) {
    if (left >= right || bit < 0) return;

    elem_t mask = 1UL << bit;
    long i = left, j = right;

    while (i <= j) {
        while (i <= right && (array[i] & mask) == 0) i++;
        while (j >= left && (array[j] & mask) != 0) j--;
        if (i < j) {
            elem_t tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
            i++; j--;
        }
    }

    bit_quicksort_seq(array, left, j, bit - 1);
    bit_quicksort_seq(array, i, right, bit - 1);
}

// ======================================================
//   WERSJA RÓWNOLEGŁA (GCD – wielordzeniowa)
// ======================================================

void bit_quicksort_par(elem_t *array, long left, long right, int bit,
                       dispatch_queue_t queue, dispatch_group_t group) {
    if (left >= right || bit < 0) return;

    elem_t mask = 1UL << bit;
    long i = left, j = right;

    while (i <= j) {
        while (i <= right && (array[i] & mask) == 0) i++;
        while (j >= left && (array[j] & mask) != 0) j--;
        if (i < j) {
            elem_t tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
            i++; j--;
        }
    }

    if ((right - left) > PARALLEL_THRESHOLD) {
        dispatch_group_async(group, queue, ^{
            bit_quicksort_par(array, left, j, bit - 1, queue, group);
        });
        dispatch_group_async(group, queue, ^{
            bit_quicksort_par(array, i, right, bit - 1, queue, group);
        });
    } else {
        bit_quicksort_seq(array, left, j, bit - 1);
        bit_quicksort_seq(array, i, right, bit - 1);
    }
}

void bit_quicksort_parallel(elem_t *array, size_t n) {
    dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    dispatch_group_t group = dispatch_group_create();

    bit_quicksort_par(array, 0, n - 1, (int)(sizeof(elem_t) * 8 - 1), queue, group);
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
}

// ======================================================
//   WERYFIKACJA I TESTY
// ======================================================

int verify_sorted(const elem_t *arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            fprintf(stderr, "Błąd sortowania przy indeksie %zu: %lu > %lu\n",
                    i, arr[i - 1], arr[i]);
            return 0;
        }
    }
    return 1;
}

int main(void) {
    elem_t *arr_seq = malloc(ARRAY_SIZE * sizeof(elem_t));
    elem_t *arr_par = malloc(ARRAY_SIZE * sizeof(elem_t));
    if (!arr_seq || !arr_par) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }

    double total_seq = 0.0, total_par = 0.0;

    for (int run = 0; run < RUNS; run++) {
        generuj_liczby_losowe(arr_seq, ARRAY_SIZE);
        for (size_t i = 0; i < ARRAY_SIZE; i++) arr_par[i] = arr_seq[i];

        // Sekwencyjny
        double start = now_sec();
        bit_quicksort_seq(arr_seq, 0, ARRAY_SIZE - 1, (int)(sizeof(elem_t) * 8 - 1));
        double end = now_sec();
        total_seq += (end - start);
        if (!verify_sorted(arr_seq, ARRAY_SIZE)) return 1;

        // Równoległy
        start = now_sec();
        bit_quicksort_parallel(arr_par, ARRAY_SIZE);
        end = now_sec();
        total_par += (end - start);
        if (!verify_sorted(arr_par, ARRAY_SIZE)) return 1;
    }

    printf("Bitowy quicksort (%d elementów, %d powtórzeń)\n", ARRAY_SIZE, RUNS);
    printf("Sekwencyjny: total %.4fs, avg %.6fs\n", total_seq, total_seq / RUNS);
    printf("Równoległy: total %.4fs, avg %.6fs\n", total_par, total_par / RUNS);
    printf("Przyspieszenie: x%.2f\n", total_seq / total_par);

    free(arr_seq);
    free(arr_par);
    return EXIT_SUCCESS;
}