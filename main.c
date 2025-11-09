#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "functions/functions.h"

// ----------------------------- CUSTOM STRUCTURES -----------------------------
typedef struct {
    elem_t *arr_base;
    elem_t *arr_bit_seq;
    elem_t *arr_bit_par;
    elem_t *arr_bit_seq_shift;
    elem_t *arr_bit_par_shift;
    elem_t *arr_std_seq;
    elem_t *arr_std_par;
} array_list;

// --------------------------------- FUNCTIONS ---------------------------------
static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void free_arrays(array_list *arrays) {
    free(arrays->arr_base);
    free(arrays->arr_bit_seq);
    free(arrays->arr_bit_par);
    free(arrays->arr_bit_seq_shift);
    free(arrays->arr_bit_par_shift);
    free(arrays->arr_std_seq);
    free(arrays->arr_std_par);
}

static int create_arrays(array_list *arrays, long arr_size) {
    arrays->arr_base            = malloc(arr_size * sizeof(elem_t));
    arrays->arr_bit_seq         = malloc(arr_size * sizeof(elem_t));
    arrays->arr_bit_par         = malloc(arr_size * sizeof(elem_t));
    arrays->arr_bit_seq_shift   = malloc(arr_size * sizeof(elem_t));
    arrays->arr_bit_par_shift   = malloc(arr_size * sizeof(elem_t));
    arrays->arr_std_seq         = malloc(arr_size * sizeof(elem_t));
    arrays->arr_std_par         = malloc(arr_size * sizeof(elem_t));
    
    if (!arrays->arr_base
        || !arrays->arr_bit_seq || !arrays->arr_bit_par
        || !arrays->arr_bit_seq_shift || !arrays->arr_bit_par_shift
        || !arrays->arr_std_seq || !arrays->arr_std_par) {
        
        free_arrays(arrays);
        perror("malloc failed");
        return -1;
    }
    return 0;
}

static void refill_arrays(array_list arrays, long arr_size){
    gen_rand_num_range(arrays.arr_base, arr_size);
    for (long i = 0; i < arr_size; i++) {
        // if (i < 1) printf("%ld\n",arrays.arr_base[i]);
        arrays.arr_bit_seq[i]       = arrays.arr_base[i];
        arrays.arr_bit_par[i]       = arrays.arr_base[i];
        arrays.arr_bit_seq_shift[i] = arrays.arr_base[i];
        arrays.arr_bit_par_shift[i] = arrays.arr_base[i];
        arrays.arr_std_seq[i]       = arrays.arr_base[i];
        arrays.arr_std_par[i]       = arrays.arr_base[i];
    }
}

static inline int initial_data_filter(elem_t* arr, long size) {
    
    elem_t greatest_in_array = 0;
    int shift = (int)(sizeof(elem_t) * 8) - 1;
    elem_t max_value = ((elem_t)1 << shift);

    for (long n = 0; n < size; ++n) {
        if (greatest_in_array < arr[n]) greatest_in_array = arr[n];
        if (max_value <= greatest_in_array) break;
    }

    int counter = 0;
    while (greatest_in_array > 0) {
        greatest_in_array >>= 1;
        ++counter;
    }
    return counter;
}

int main(int argc, char *argv[]) {

    double total_bit_seq = 0.0;
    double total_bit_par = 0.0;
    double total_bit_seq_new_start = 0.0;
    double total_bit_par_new_start = 0.0;
    double total_std_seq = 0.0;
    double total_std_par = 0.0;
    double start, end;
    int enum_size = (int)sizeof(elem_t) << 3;

    long size = 5000000;
    long runs = 100;

    if (argc >= 2){
        size = atol(argv[1]);
    }
    if (argc >= 3) {
        runs = atol(argv[2]);
    }

    array_list arrays;
    if (create_arrays(&arrays, size) != 0) return EXIT_FAILURE;
    
    printf("\nRuning ");
    fflush(stdout);

    for (int r = 0; r < runs; r++) {
        fprintf(stdout, ".");
        fflush(stdout);
        refill_arrays(arrays, size);

        // 1.  Binary sequential
        start = now_sec();
        qs_bin_sequential(arrays.arr_bit_seq, size - 1, enum_size);
        end = now_sec();
        total_bit_seq += (end - start);
        if (!verify_sorted(arrays.arr_bit_seq, size)) {
            fprintf(stderr,"Error with sorting binary sequential\n");
            break; 
        }

        // 2. Binary parallel
        start = now_sec();
        qs_bin_parallel(arrays.arr_bit_par, size - 1, enum_size);
        end = now_sec();
        total_bit_par += (end - start);
        if (!verify_sorted(arrays.arr_bit_par, size)) {
            fprintf(stderr,"Error with sorting binary parallel\n");
            break;
        }
        
        // 3. Binary sequential with initial reduction of starting bit
        start = now_sec();
        qs_bin_sequential(arrays.arr_bit_seq_shift, size - 1, initial_data_filter(arrays.arr_bit_seq_shift, size));
        end = now_sec();
        total_bit_seq_new_start += (end - start);
        if (!verify_sorted(arrays.arr_bit_seq_shift, size)) {
            fprintf(stderr,"Error with sorting Binary sequential with initial reduction of starting bit\n");
            break; 
        }

        // 4. Binary parallel with initial reduction of starting bit
        start = now_sec();
        qs_bin_parallel(arrays.arr_bit_par_shift, size - 1, initial_data_filter(arrays.arr_bit_par_shift, size));
        end = now_sec();
        total_bit_par_new_start += (end - start);
        if (!verify_sorted(arrays.arr_bit_par_shift, size)) {
            fprintf(stderr,"Error with sorting binary parallel with initial reduction of starting bit\n");
            break;
        }

        // 5. Clasical sequential Quicksort
        start = now_sec();
        qs_std_sequential(arrays.arr_std_seq, size - 1);
        end = now_sec();
        total_std_seq += (end - start);
        if (!verify_sorted(arrays.arr_std_seq, size)) {
            fprintf(stderr,"Error with sorting clasical sequential Quicksort\n");
            break;
        }

        // 6. Clasical parallel quicksort
        start = now_sec();
        qs_std_parallel(arrays.arr_std_par, size -1);
        end = now_sec();
        total_std_par += (end - start);
        if (!verify_sorted(arrays.arr_std_par, size)) {
            fprintf(stderr,"Error with sorting clasical parallel quicksort\n");
            break;
        }
    }
    
    printf("\x1b[2K\r");
    printf("============================================================\n");
    printf("Benchmark (%ld runs, %ld elements)\n", runs, size);
    printf("------------------------------------------------------------\n");
    printf("Binary (1 core):                     %.4fs | avg %.6fs\n", total_bit_seq, total_bit_seq / runs);
    printf("Binary (multicore):                  %.4fs | avg %.6fs\n", total_bit_par, total_bit_par / runs);
    printf("Binary + reduced range (1 core):     %.4fs | avg %.6fs\n", total_bit_seq_new_start, total_bit_seq_new_start / runs);
    printf("Binary + reduced range (multicore):  %.4fs | avg %.6fs\n", total_bit_par_new_start, total_bit_par_new_start / runs);
    printf("Clasical (1 core):                   %.4fs | avg %.6fs\n", total_std_seq, total_std_seq / runs);
    printf("Clasical (multicore):                %.4fs | avg %.6fs\n", total_std_par, total_std_par / runs);
    printf("------------------------------------------------------------\n");
    printf("Sequential speedup:                       x%.2f\n", total_std_seq / total_bit_seq);
    printf("Sequential speedup after reducing range:  x%.2f\n", total_std_seq / total_bit_seq_new_start);
    printf("Parallel speedup:                         x%.2f\n", total_std_par / total_bit_par);
    printf("Parallel speedup after reducing range:    x%.2f\n", total_std_par / total_bit_par_new_start);
    printf("============================================================\n");

    free_arrays(&arrays);
    return 0;
}
