#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "functions/functions.h"


// ----------------------------------- CONFIG ----------------------------------
#define ARRAY_SIZE 500000      // liczba elementów
#define RUNS 100               // liczba powtórzeń
#define PARALLEL_THRESHOLD 100000
#define SWAP(a, b) { elem_t tmp = (a); (a) = (b); (b) = tmp; }
// ------------------------------------------------

static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(void) {
    elem_t *arr_base    = malloc(ARRAY_SIZE * sizeof(elem_t));
    elem_t *arr_bit_seq = malloc(ARRAY_SIZE * sizeof(elem_t));
    elem_t *arr_bit_par = malloc(ARRAY_SIZE * sizeof(elem_t));
    elem_t *arr_std_seq = malloc(ARRAY_SIZE * sizeof(elem_t));
    elem_t *arr_std_par = malloc(ARRAY_SIZE * sizeof(elem_t));
    
    if (!arr_base || !arr_bit_seq || !arr_bit_par || !arr_std_seq || !arr_std_par) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }

    double total_bit_seq = 0.0;
    double total_bit_par = 0.0;
    double total_std_seq = 0.0;
    double total_std_par = 0.0;
    double start, end;

    for (int r = 0; r < RUNS; r++) {
        gen_rand_num(arr_base, ARRAY_SIZE);

        // Przygotowanie czystych kopii
        for (size_t i = 0; i < ARRAY_SIZE; i++) {
            arr_bit_seq[i] = arr_base[i];
            arr_bit_par[i] = arr_base[i];
            arr_std_seq[i] = arr_base[i];
            arr_std_par[i] = arr_base[i];
        }

        // 1. Bitowy sekwencyjny
        start = now_sec();
        qs_bin_single(arr_bit_seq, 0, ARRAY_SIZE - 1, (int)(sizeof(elem_t)*8 - 1));
        end = now_sec();
        total_bit_seq += (end - start);
        if (!verify_sorted(arr_bit_seq, ARRAY_SIZE)) { fprintf(stderr,"Błąd bit_seq\n"); break; }

        // 2. Bitowy równoległy
        start = now_sec();
        qs_bin_parallel(arr_bit_par, ARRAY_SIZE, PARALLEL_THRESHOLD);
        end = now_sec();
        total_bit_par += (end - start);
        if (!verify_sorted(arr_bit_par, ARRAY_SIZE)) { fprintf(stderr,"Błąd bit_par\n"); break; }

        // 3. Klasyczny sekwencyjny
        start = now_sec();
        qs_std_single(arr_std_seq, 0, ARRAY_SIZE - 1);
        end = now_sec();
        total_std_seq += (end - start);
        if (!verify_sorted(arr_std_seq, ARRAY_SIZE)) { fprintf(stderr,"Błąd std_seq\n"); break; }

        // 4. Klasyczny równoległy
        start = now_sec();
        qs_std_parallel(arr_std_par, ARRAY_SIZE, PARALLEL_THRESHOLD);
        end = now_sec();
        total_std_par += (end - start);
        if (!verify_sorted(arr_std_par, ARRAY_SIZE)) { fprintf(stderr,"Błąd std_par\n"); break; }
    }

    printf("============================================================\n");
    printf("Benchmark sortowań (%d powtórzeń, %d elementów)\n", RUNS, ARRAY_SIZE);
    printf("------------------------------------------------------------\n");
    printf("Bitowy quicksort (1 rdzeń): total %.4fs | avg %.6fs\n", total_bit_seq, total_bit_seq / RUNS);
    printf("Bitowy quicksort (multi):  total %.4fs | avg %.6fs\n", total_bit_par, total_bit_par / RUNS);
    printf("Klasyczny quicksort (1 rdzeń): total %.4fs | avg %.6fs\n", total_std_seq, total_std_seq / RUNS);
    printf("Klasyczny quicksort (multi):  total %.4fs | avg %.6fs\n", total_std_par, total_std_par / RUNS);
    printf("------------------------------------------------------------\n");
    printf("Przyspieszenie bitowego:  x%.2f\n", total_std_seq / total_bit_seq);
    printf("Przyspieszenie klasycznego: x%.2f\n", total_std_par / total_bit_par);
    printf("============================================================\n");

    free(arr_base); free(arr_bit_seq); free(arr_bit_par);
    free(arr_std_seq); free(arr_std_par);
    return 0;
}
