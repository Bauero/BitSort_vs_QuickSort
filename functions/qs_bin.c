#include <stdlib.h>
#include <dispatch/dispatch.h>
#include "__init__.h"

static inline void qs_bin_seq(elem_t *arr, long left, long right, int bit) {
    if (left >= right || bit < 0) return;

    elem_t mask = 1UL << bit;
    long i = left, j = right;

    while (i <= j) {
        while (i <= right && (arr[i] & mask) == 0) ++i;
        while (j >= left && (arr[j] & mask) != 0) --j;

        if (i < j) {
            SWAP(arr[i], arr[j]);
            ++i;
            if (0 > --j) break; // prevent underflow
        }
    }
    if (left < j) qs_bin_seq(arr, left, j, bit - 1);
    if (i < right) qs_bin_seq(arr, i, right, bit - 1);
}

static inline void qs_bin_par(elem_t *arr, long left, long right, int bit,
                       dispatch_queue_t queue, dispatch_group_t group) {
    if (left >= right || bit < 0) return;
    elem_t mask = 1UL << bit;
    long i = left, j = right;
    while (i <= j) {
        while (i <= right && (arr[i] & mask) == 0) ++i;
        while (j >= left && (arr[j] & mask) != 0) --j;
        if (i < j) {
            SWAP(arr[i], arr[j]);
            i++;
            if (j == 0) break; // prevent underflow
            j--;
        }
    }
    if ((right - left) > PARALLEL_THRESHOLD) {
        dispatch_group_async(group, queue, ^{
            qs_bin_par(arr, left, j, bit - 1, queue, group);
        });
        dispatch_group_async(group, queue, ^{
            qs_bin_par(arr, i, right, bit - 1, queue, group);
        });
    } else {
        qs_bin_seq(arr, left, j, bit - 1);
        qs_bin_seq(arr, i, right, bit - 1);
    }
}

void qs_bin_parallel(elem_t *arr, size_t n) {
    dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    dispatch_group_t group = dispatch_group_create();
    qs_bin_par(arr, 0, n - 1, (int)(sizeof(elem_t)*8 - 1), queue, group);
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
}

void qs_bin_sequential(elem_t *arr, long left, long right, int bit){
    qs_bin_seq(arr, left, right, bit);
}
