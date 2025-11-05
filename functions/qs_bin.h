#ifndef QS_BIN_H
#define QS_BIN_H

typedef unsigned long elem_t;

void qs_bin_single(elem_t *arr, long left, long right, int bit);
void qs_bin_parallel(elem_t *arr, size_t n, int parallel_threshold);

#endif