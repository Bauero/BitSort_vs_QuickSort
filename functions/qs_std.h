#ifndef QS_STD_H
#define QS_STD_H

#include "__init__.h"

void qs_std_single(elem_t *a, long left, long right);
void qs_std_parallel(elem_t *a, size_t n, int parallel_threshold);

#endif