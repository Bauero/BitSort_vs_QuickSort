#include <stdio.h>
#include <stdlib.h>
#include "__init__.h"

void gen_rand_num(elem_t *arr, long n) {
    for (long i = 0; i < n; i++)
        arr[i] = (elem_t)arc4random();
}

void gen_rand_num_range(elem_t *arr, long n) {
    for (long i = 0; i < n; i++){
        elem_t value = 0;
        for (int byte = 0; byte < (int)sizeof(elem_t); ++byte){
            value |= ((elem_t)arc4random_uniform(256) << (8 * byte));
        }
        arr[i] = value;
    }
}

int verify_sorted(const elem_t *arr, long n) {
    for (long i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            fprintf(stderr, "Błąd sortowania przy indeksie %ld: %llu > %llu\n",
                    i, (unsigned long long)arr[i - 1], (unsigned long long)arr[i]);
            return 0;
        }
    }
    return 1;
}
