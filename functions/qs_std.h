#ifndef QS_STD_H
#define QS_STD_H

typedef unsigned long elem_t;

void qs_std_single(elem_t *a, long left, long right);
void qs_std_parallel(elem_t *a, size_t n, int parallel_threshold);

#endif