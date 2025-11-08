#include <dispatch/dispatch.h>
#include "__init__.h"

static inline elem_t median_of_three(elem_t *a, long x, long y, long z) {
    elem_t A = a[x], B = a[y], C = a[z];
    if ((A < B && B < C) || (C < B && B < A)) return B;
    else if ((B < A && A < C) || (C < A && A < B)) return A;
    else return C;
}

static inline void qs_std_seq(elem_t *arr, long left, long right) {
    if (left >= right) return;
    elem_t pivot = median_of_three(arr, left, (left + right)/2, right);
    long i = left, j = right;
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            SWAP(arr[i], arr[j]);
            i++;
            if (j == 0) break; // prevent underflow
            j--;
        }
    }
    if (left < j) qs_std_seq(arr, left, j);
    if (i < right) qs_std_seq(arr, i, right);
}

static inline void qs_std_par(elem_t *arr, long left, long right,
                              dispatch_queue_t queue, dispatch_group_t group) {
    if (left >= right) return;
    elem_t pivot = median_of_three(arr, left, (left + right)/2, right);
    long i = left, j = right;
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            SWAP(arr[i], arr[j]);
            i++;
            if (j == 0) break; // prevent underflow
            j--;
        }
    }
    if ((right - left) > PARALLEL_THRESHOLD) {
        dispatch_group_async(group, queue, ^{
            qs_std_par(arr, left, j, queue, group);
        });
        dispatch_group_async(group, queue, ^{
            qs_std_par(arr, i, right, queue, group);
        });
    } else {
        if (left < j) qs_std_seq(arr, left, j);
        if (i < right) qs_std_seq(arr, i, right);
    }
}

void qs_std_parallel(elem_t *a, long n) {
    dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    dispatch_group_t group = dispatch_group_create();
    qs_std_par(a, 0, n, queue, group);
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
}

void qs_std_sequential(elem_t *a, long n){
    qs_std_seq(a, 0, n);
}