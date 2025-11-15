#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "functions/functions.h"

// ----------------------------- CUSTOM STRUCTURES -----------------------------
typedef struct {
    elem_t *master_array;
    elem_t *sorting_array;
} array_list;

// --------------------------------- FUNCTIONS ---------------------------------
static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void free_arrays(array_list *arrays) {
    free(arrays->master_array);
    free(arrays->sorting_array);
}

static int create_arrays(array_list *arrays, long arr_size) {
    arrays->master_array  = malloc(arr_size * sizeof(elem_t));
    arrays->sorting_array = malloc(arr_size * sizeof(elem_t));
    
    if (!arrays->master_array || !arrays->sorting_array) {
        free_arrays(arrays);
        perror("malloc failed");
        return -1;
    }
    return 0;
}

static void reset_sorting_array(array_list arrays, long arr_size) {
    for (long i = 0; i < arr_size; i++) {
        arrays.sorting_array[i] = arrays.master_array[i];
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
    double total_bit_alt_seq = 0.0;
    double total_bit_alt_par = 0.0;
    double total_bit_alt_seq_new_start = 0.0;
    double total_bit_alt_par_new_start = 0.0;
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
        gen_rand_num_range(arrays.master_array, size);

        // 1.  Binary sequential
        reset_sorting_array(arrays, size);
        start = now_sec();
        qs_bin_sequential(arrays.sorting_array, size - 1, enum_size);
        end = now_sec();
        total_bit_seq += (end - start);
        if (!verify_sorted(arrays.sorting_array, size)) {
            fprintf(stderr,"Error with sorting binary sequential\n");
            break; 
        }

        // 2. Binary parallel
        reset_sorting_array(arrays, size);
        start = now_sec();
        qs_bin_parallel(arrays.sorting_array, size - 1, enum_size);
        end = now_sec();
        total_bit_par += (end - start);
        if (!verify_sorted(arrays.sorting_array, size)) {
            fprintf(stderr,"Error with sorting binary parallel\n");
            break;
        }
        
        // 3. Binary sequential with initial reduction of starting bit
        reset_sorting_array(arrays, size);
        start = now_sec();
        qs_bin_sequential(arrays.sorting_array, size - 1, initial_data_filter(arrays.sorting_array, size));
        end = now_sec();
        total_bit_seq_new_start += (end - start);
        if (!verify_sorted(arrays.sorting_array, size)) {
            fprintf(stderr,"Error with sorting Binary sequential with initial reduction of starting bit\n");
            break; 
        }

        // 4. Binary parallel with initial reduction of starting bit
        reset_sorting_array(arrays, size);
        start = now_sec();
        qs_bin_parallel(arrays.sorting_array, size - 1, initial_data_filter(arrays.sorting_array, size));
        end = now_sec();
        total_bit_par_new_start += (end - start);
        if (!verify_sorted(arrays.sorting_array, size)) {
            fprintf(stderr,"Error with sorting binary parallel with initial reduction of starting bit\n");
            break;
        }
        
        // 5.  Alternative binary sequential
        reset_sorting_array(arrays, size);
        start = now_sec();
        qs_bin_sequential(arrays.sorting_array, size - 1, enum_size);
        end = now_sec();
        total_bit_alt_seq += (end - start);
        if (!verify_sorted(arrays.sorting_array, size)) {
            fprintf(stderr,"Error with sorting binary sequential\n");
            break; 
        }

        // 6. Alternative binary parallel
        reset_sorting_array(arrays, size);
        start = now_sec();
        qs_bin_parallel(arrays.sorting_array, size - 1, enum_size);
        end = now_sec();
        total_bit_alt_par += (end - start);
        if (!verify_sorted(arrays.sorting_array, size)) {
            fprintf(stderr,"Error with sorting binary parallel\n");
            break;
        }
        
        // 7. Alternative binary sequential with initial reduction of starting bit
        reset_sorting_array(arrays, size);
        start = now_sec();
        qs_bin_sequential(arrays.sorting_array, size - 1, initial_data_filter(arrays.sorting_array, size));
        end = now_sec();
        total_bit_alt_seq_new_start += (end - start);
        if (!verify_sorted(arrays.sorting_array, size)) {
            fprintf(stderr,"Error with sorting Binary sequential with initial reduction of starting bit\n");
            break; 
        }

        // 8. Alternative binary parallel with initial reduction of starting bit
        reset_sorting_array(arrays, size);
        start = now_sec();
        qs_bin_parallel(arrays.sorting_array, size - 1, initial_data_filter(arrays.sorting_array, size));
        end = now_sec();
        total_bit_alt_par_new_start += (end - start);
        if (!verify_sorted(arrays.sorting_array, size)) {
            fprintf(stderr,"Error with sorting binary parallel with initial reduction of starting bit\n");
            break;
        }

        // 9. Clasical sequential Quicksort
        reset_sorting_array(arrays, size);
        start = now_sec();
        qs_std_sequential(arrays.sorting_array, size - 1);
        end = now_sec();
        total_std_seq += (end - start);
        if (!verify_sorted(arrays.sorting_array, size)) {
            fprintf(stderr,"Error with sorting clasical sequential Quicksort\n");
            break;
        }

        //10. Clasical parallel quicksort
        start = now_sec();
        qs_std_parallel(arrays.sorting_array, size -1);
        end = now_sec();
        total_std_par += (end - start);
        if (!verify_sorted(arrays.sorting_array, size)) {
            fprintf(stderr,"Error with sorting clasical parallel quicksort\n");
            break;
        }
    }
    
    printf("\x1b[2K\r");
    printf("============================================================\n");
    printf("Benchmark (%ld runs, %ld elements)\n", runs, size);
    printf("------------------------------------------------------------\n");
    printf("Binary (1 core):                                 %.4fs | avg %.6fs\n", total_bit_seq, total_bit_seq / runs);
    printf("Binary (multicore):                              %.4fs | avg %.6fs\n", total_bit_par, total_bit_par / runs);
    printf("Binary + reduced range (1 core):                 %.4fs | avg %.6fs\n", total_bit_seq_new_start, total_bit_seq_new_start / runs);
    printf("Binary + reduced range (multicore):              %.4fs | avg %.6fs\n", total_bit_par_new_start, total_bit_par_new_start / runs);
    printf("Alternative binary (1 core):                     %.4fs | avg %.6fs\n", total_bit_alt_seq, total_bit_alt_seq / runs);
    printf("Alternative binary (multicore):                  %.4fs | avg %.6fs\n", total_bit_alt_par, total_bit_alt_par / runs);
    printf("Alternative binary + reduced range (1 core):     %.4fs | avg %.6fs\n", total_bit_alt_seq_new_start, total_bit_alt_seq_new_start / runs);
    printf("Alternative binary + reduced range (multicore):  %.4fs | avg %.6fs\n", total_bit_alt_par_new_start, total_bit_alt_par_new_start / runs);
    printf("Clasical (1 core):                               %.4fs | avg %.6fs\n", total_std_seq, total_std_seq / runs);
    printf("Clasical (multicore):                            %.4fs | avg %.6fs\n", total_std_par, total_std_par / runs);
    printf("------------------------------------------------------------\n");
    printf("Sequential speedup:                                     x%.2f\n", total_std_seq / total_bit_seq);
    printf("Sequential speedup after reducing range:                x%.2f\n", total_std_seq / total_bit_seq_new_start);
    printf("Alternative sequential speedup:                         x%.2f\n", total_std_seq / total_bit_alt_seq);
    printf("Alternative equential speedup after reducing range:     x%.2f\n", total_std_seq / total_bit_alt_seq_new_start);
    printf("Parallel speedup:                                       x%.2f\n", total_std_par / total_bit_par);
    printf("Parallel speedup after reducing range:                  x%.2f\n", total_std_par / total_bit_par_new_start);
    printf("Alternative parallel speedup:                           x%.2f\n", total_std_par / total_bit_alt_par);
    printf("Alternative parallel speedup after reducing range:      x%.2f\n", total_std_par / total_bit_alt_par_new_start);
    printf("============================================================\n");

    free_arrays(&arrays);
    return 0;
}
