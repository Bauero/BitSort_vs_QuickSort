#include <dispatch/dispatch.h>

typedef unsigned long elem_t;

static inline elem_t median_of_three(elem_t *a, size_t x, size_t y, size_t z) {
    elem_t A = a[x], B = a[y], C = a[z];
    if ((A < B && B < C) || (C < B && B < A)) return B;
    else if ((B < A && A < C) || (C < A && A < B)) return A;
    else return C;
}

static inline void qs_std_seq(elem_t *a, long left, long right) {
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

static inline void qs_std_par(elem_t *a, long left, long right,
                   dispatch_queue_t queue, dispatch_group_t group,
                    int parallel_threshold) {
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
    if ((right - left) > parallel_threshold) {
        dispatch_group_async(group, queue, ^{
            qs_std_par(a, left, j, queue, group, parallel_threshold);
        });
        dispatch_group_async(group, queue, ^{
            qs_std_par(a, i, right, queue, group, parallel_threshold);
        });
    } else {
        if (left < j) qs_std_seq(a, left, j);
        if (i < right) qs_std_seq(a, i, right);
    }
}

void qs_std_parallel(elem_t *a, size_t n, int parallel_threshold) {
    dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    dispatch_group_t group = dispatch_group_create();
    qs_std_par(a, 0, n - 1, queue, group, parallel_threshold);
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
}

void qs_std_single(elem_t *a, long left, long right){
    qs_std_seq(a, left, right);
}