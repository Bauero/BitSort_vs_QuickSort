#include <stdlib.h>

typedef unsigned long elem_t;

static inline elem_t median_of_three(elem_t *a, size_t x, size_t y, size_t z) {
    elem_t A = a[x], B = a[y], C = a[z];
    if ((A < B && B < C) || (C < B && B < A)) return B;
    else if ((B < A && A < C) || (C < A && A < B)) return A;
    else return C;
}
void qs_std_seq(elem_t *a, long left, long right) {
    if (left >= right) return;
    elem_t pivot = median_of_three(a, left, (left + right)/2, right);
    long i = left, j = right;
    while (i <= j) {
        while (a[i] < pivot) i++;
        while (a[j] > pivot) j--;
        if (i <= j) {
            elem_t tmp = a[i]; a[i] = a[j]; a[j] = tmp;
            i++; j--;
        }
    }
    if (left < j) qs_std_seq(a, left, j);
    if (i < right) qs_std_seq(a, i, right);
}
