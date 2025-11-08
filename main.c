#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "functions/functions.h"

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

int create_arrays(array_list *arrays, long arr_size) {
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

void refill_arrays(array_list arrays, long arr_size){
    gen_rand_num_range(arrays.arr_base, arr_size, (long)((sizeof(elem_t) << 3) - 1));
    for (long i = 0; i < arr_size; i++) {
        arrays.arr_bit_seq[i] = arrays.arr_base[i];
        arrays.arr_bit_par[i] = arrays.arr_base[i];
        arrays.arr_std_seq[i] = arrays.arr_base[i];
        arrays.arr_std_par[i] = arrays.arr_base[i];
    }
}

int main(int argc, char *argv[]) {
    
    long size = 5000000;
    // long seq_lim = 10;
    long runs = 100;

    if (argc >= 2){
        size = atol(argv[1]);
        printf("Size %ld\n", size);
        // printf("seq_lim %ld\n", seq_lim);
    }
    // if (argc >= 3) {
    //     seq_lim = atol(argv[2]);
    //     printf("seq_lim %ld\n", seq_lim);
    // }
    if (argc >= 3) {
        runs = atol(argv[3]);
        printf("runs %ld\n", runs);
    }

    array_list arrays;
    if (create_arrays(&arrays, size) != 0) return EXIT_FAILURE;

    double total_bit_seq = 0.0;
    double total_bit_par = 0.0;
    double total_std_seq = 0.0;
    double total_std_par = 0.0;
    double start, end;

    for (int r = 0; r < runs; r++) {
        refill_arrays(arrays, size);

        // 1. Bitowy sekwencyjny
        start = now_sec();
        qs_bin_sequential(arrays.arr_bit_seq, size - 1);
        end = now_sec();
        total_bit_seq += (end - start);
        if (!verify_sorted(arrays.arr_bit_seq, size)) { fprintf(stderr,"Błąd bit_seq\n"); break; }

        // 2. Bitowy równoległy
        start = now_sec();
        qs_bin_parallel(arrays.arr_bit_par, size - 1);
        end = now_sec();
        total_bit_par += (end - start);
        if (!verify_sorted(arrays.arr_bit_par, size)) { fprintf(stderr,"Błąd bit_par\n"); break; }

        // 3. Klasyczny sekwencyjny
        start = now_sec();
        qs_std_sequential(arrays.arr_std_seq, size - 1);
        end = now_sec();
        total_std_seq += (end - start);
        if (!verify_sorted(arrays.arr_std_seq, size)) { fprintf(stderr,"Błąd std_seq\n"); break; }

        // 4. Klasyczny równoległy
        start = now_sec();
        qs_std_parallel(arrays.arr_std_par, size -1);
        end = now_sec();
        total_std_par += (end - start);
        if (!verify_sorted(arrays.arr_std_par, size)) { fprintf(stderr,"Błąd std_par\n"); break; }
    }

    printf("============================================================\n");
    printf("Benchmark sortowań (%ld powtórzeń, %ld elementów)\n", runs, size);
    printf("------------------------------------------------------------\n");
    printf("Bitowy quicksort (1 rdzeń): total %.4fs | avg %.6fs\n", total_bit_seq, total_bit_seq / runs);
    printf("Bitowy quicksort (multi):  total %.4fs | avg %.6fs\n", total_bit_par, total_bit_par / runs);
    printf("Klasyczny quicksort (1 rdzeń): total %.4fs | avg %.6fs\n", total_std_seq, total_std_seq / runs);
    printf("Klasyczny quicksort (multi):  total %.4fs | avg %.6fs\n", total_std_par, total_std_par / runs);
    printf("------------------------------------------------------------\n");
    printf("Przyspieszenie sekwencyjne:  x%.2f\n", total_std_seq / total_bit_seq);
    printf("Przyspieszenie równoległe: x%.2f\n", total_std_par / total_bit_par);
    printf("============================================================\n");

    free_arrays(&arrays);
    return 0;
}
