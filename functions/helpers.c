#include <stdio.h>
#include <stdlib.h>
#include "__init__.h"

void gen_rand_num(elem_t *arr, long n) {
    for (elem_t i = 0; i < n; i++)
        arr[i] = arc4random();
}

void gen_rand_num_range(elem_t *arr, long n, long limit) {
    for (elem_t i = 0; i < n; i++)
        arr[i] = arc4random_uniform(limit);
}

int verify_sorted(const elem_t *arr, long n) {
    for (elem_t i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            fprintf(stderr, "Błąd sortowania przy indeksie %zu: %lu > %lu\n",
                    i, arr[i - 1], arr[i]);
            return 0;
        }
    }
    return 1;
}
