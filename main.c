#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "functions/functions.h"


// ----------------------------------- CONFIG ----------------------------------
#define ARRAY_SIZE 500000      // liczba elementów
#define RUNS 100               // liczba powtórzeń
#define PARALLEL_THRESHOLD 100000
#define SWAP(a, b) { elem_t tmp = (a); (a) = (b); (b) = tmp; }


// ----------------------------- CUSTOM STRUCTURES -----------------------------
typedef struct {
    elem_t *arr_base;
    elem_t *arr_bit_seq;
    elem_t *arr_bit_par;
    elem_t *arr_std_seq;
    elem_t *arr_std_par;
} array_list;

// --------------------------------- FUNCTIONS ---------------------------------
static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int create_arrays(array_list *arrays, int arr_size) {
    arrays->arr_base    = malloc(arr_size * sizeof(elem_t));
    arrays->arr_bit_seq = malloc(arr_size * sizeof(elem_t));
    arrays->arr_bit_par = malloc(arr_size * sizeof(elem_t));
    arrays->arr_std_seq = malloc(arr_size * sizeof(elem_t));
    arrays->arr_std_par = malloc(arr_size * sizeof(elem_t));
    
    if (!arrays->arr_base
        || !arrays->arr_bit_seq || !arrays->arr_bit_par 
        || !arrays->arr_std_seq || !arrays->arr_std_par) {
        
        free(arrays->arr_base);
        free(arrays->arr_bit_seq);
        free(arrays->arr_bit_par);
        free(arrays->arr_std_seq);
        free(arrays->arr_std_par);
        
        perror("malloc failed");
        return -1;
    }
    return 0;
}

void free_arrays(array_list *arrays) {
    free(arrays->arr_base);
    free(arrays->arr_bit_seq);
    free(arrays->arr_bit_par);
    free(arrays->arr_std_seq);
    free(arrays->arr_std_par);
}

void refill_arrays(array_list arrays, int arr_size){
    gen_rand_num(arrays.arr_base, arr_size);
    for (size_t i = 0; i < arr_size; i++) {
        arrays.arr_bit_seq[i] = arrays.arr_base[i];
        arrays.arr_bit_par[i] = arrays.arr_base[i];
        arrays.arr_std_seq[i] = arrays.arr_base[i];
        arrays.arr_std_par[i] = arrays.arr_base[i];
    }
}

int main() {
    long size = 5000000;
    array_list arrays;
    if (create_arrays(&arrays, size) != 0) return EXIT_FAILURE;

    double total_bit_seq = 0.0;
    double total_bit_par = 0.0;
    double total_std_seq = 0.0;
    double total_std_par = 0.0;
    double start, end;

    for (int r = 0; r < RUNS; r++) {
        refill_arrays(arrays, ARRAY_SIZE);

        // 1. Bitowy sekwencyjny
        start = now_sec();
        qs_bin_single(arrays.arr_bit_seq, 0, ARRAY_SIZE - 1, (int)(sizeof(elem_t)*8 - 1));
        end = now_sec();
        total_bit_seq += (end - start);
        if (!verify_sorted(arrays.arr_bit_seq, ARRAY_SIZE)) { fprintf(stderr,"Błąd bit_seq\n"); break; }

        // 2. Bitowy równoległy
        start = now_sec();
        qs_bin_parallel(arrays.arr_bit_par, ARRAY_SIZE, PARALLEL_THRESHOLD);
        end = now_sec();
        total_bit_par += (end - start);
        if (!verify_sorted(arrays.arr_bit_par, ARRAY_SIZE)) { fprintf(stderr,"Błąd bit_par\n"); break; }

        // 3. Klasyczny sekwencyjny
        start = now_sec();
        qs_std_single(arrays.arr_std_seq, 0, ARRAY_SIZE - 1);
        end = now_sec();
        total_std_seq += (end - start);
        if (!verify_sorted(arrays.arr_std_seq, ARRAY_SIZE)) { fprintf(stderr,"Błąd std_seq\n"); break; }

        // 4. Klasyczny równoległy
        start = now_sec();
        qs_std_parallel(arrays.arr_std_par, ARRAY_SIZE, PARALLEL_THRESHOLD);
        end = now_sec();
        total_std_par += (end - start);
        if (!verify_sorted(arrays.arr_std_par, ARRAY_SIZE)) { fprintf(stderr,"Błąd std_par\n"); break; }
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

    free_arrays(&arrays);
    return 0;
}
