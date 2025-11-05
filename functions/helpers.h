#ifndef HELPERS_H
#define HELPERS_H

#include "__init__.h"

int verify_sorted(const elem_t *arr, size_t n);
void gen_rand_num(elem_t *arr, size_t n);
void gen_rand_num_range(elem_t *arr, size_t n, int limit);

#endif