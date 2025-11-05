#include <dispatch/dispatch.h>

typedef unsigned long elem_t;

void gs_bin_seq(elem_t *arr, long left, long right, int bit);

void qs_bin_par(elem_t *arr, long left, long right, int bit,
                       dispatch_queue_t queue, dispatch_group_t group,
                        int parallel_threshold) {
    if (left >= right || bit < 0) return;
    elem_t mask = 1UL << bit;
    long i = left, j = right;
    while (i <= j) {
        while (i <= right && (arr[i] & mask) == 0) i++;
        while (j >= left && (arr[j] & mask) != 0) j--;
        if (i < j) {
            elem_t tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
            i++; j--;
        }
    }
    if ((right - left) > parallel_threshold) {
        dispatch_group_async(group, queue, ^{
            qs_bin_par(arr, left, j, bit - 1, queue, group, parallel_threshold);
        });
        dispatch_group_async(group, queue, ^{
            qs_bin_par(arr, i, right, bit - 1, queue, group, parallel_threshold);
        });
    } else {
        gs_bin_seq(arr, left, j, bit - 1);
        gs_bin_seq(arr, i, right, bit - 1);
    }
}
void qs_bin_parallel(elem_t *arr, size_t n, int parallel_threshold) {
    dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    dispatch_group_t group = dispatch_group_create();
    qs_bin_par(arr, 0, n - 1, (int)(sizeof(elem_t)*8 - 1), queue, group, parallel_threshold);
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
}
