#include <stdlib.h>
#include <dispatch/dispatch.h>
#include "__init__.h"

/**
 * @brief Recursive binary quicksort implementation that sorts array by comparing bits
 * 
 * This function implements a binary quicksort algorithm that partitions elements
 * based on the value of specific bits, starting from the most significant bit
 * and moving towards less significant ones. It recursively sorts subarrays
 * using the same approach. Implementations is modification of quicksort algorithm
 * but instead of calculating pivot, we divide by lesser power of 2.
 * 
 * @param arr Pointer to the array of elements to be sorted
 * @param left Starting index of the subarray to be sorted
 * @param right Ending index of the subarray to be sorted
 * @param bit Current bit position being examined (starting from MSB)
 * 
 * @note The function uses elem_t type defined in "__init__.h"
 * @note The function uses SWAP macro for swapping elements
 * 
 * Time Complexity: O(n log n) on average
 * Space Complexity: O(log n) due to recursion stack
 */
static inline void qs_bin_seq(elem_t *arr, long left, long right, int bit) {
    if (left >= right) return;
    if (bit < 0) return; /* stop when no more bits */
    elem_t mask = ((elem_t)1 << bit);
    long i = left, j = right;
    while (i <= j) {
        while (i <= right && (arr[i] & mask) == 0) ++i;
        while (j >= left && (arr[j] & mask) != 0) --j;
        
        if (i < j) {
            SWAP(arr[i], arr[j]);
            i++;
            if (j == 0) break; // prevent underflow
            j--;
        } else break;

    }
    if (left < j) qs_bin_seq(arr, left, j, bit - 1);
    if (i < right) qs_bin_seq(arr, i, right, bit - 1);
}

/**
 * @brief Parallelized implementation of `qs_bin_seq`
 *
 * In-place, modified version of qs_bin_seq that provides an option to
 * execute recursive work in parallel. The algorithm uses a binary
 * partitioning strategy (as in the qs_bin_seq implementation) but
 * introduces a concurrency control so that large subproblems may be
 * processed concurrently while small subproblems remain sequential to
 * avoid excessive tasking overhead.
 *
 * @param arr (`elem_t*`): pointer to the first element of the sequence to be
 *       partitioned/sorted. Must be non-NULL.
 * @param left (`size_t / index type`): index of the first element in the
 *       (inclusive) range to sort.
 * @param right (`size_t / index type`): index one-past-the-last (exclusive) or
 *       last element (inclusive) depending on the project's indexing
 *       convention; the caller must follow the same convention used by
 *       qs_bin_seq.
 */
static inline void qs_bin_par(elem_t *arr, long left, long right, int bit,
                              dispatch_queue_t queue, dispatch_group_t group) {
    if (left >= right) return;
    if (bit < 0) return;
    elem_t mask = ((elem_t)1 << bit);
    long i = left, j = right;
    while (i <= j) {
        while (i <= right && (arr[i] & mask) == 0) ++i;
        while (j >= left && (arr[j] & mask) != 0) --j;
        if (i < j) {
            SWAP(arr[i], arr[j]);
            i++;
            if (j == 0) break; // prevent underflow
            j--;
        } else break;
    }
    if ((right - left) > PARALLEL_THRESHOLD) {
        dispatch_group_async(group, queue, ^{
            qs_bin_par(arr, left, j, bit - 1, queue, group);
        });
        dispatch_group_async(group, queue, ^{
            qs_bin_par(arr, i, right, bit - 1, queue, group);
        });
    } else {
        if (left < j) qs_bin_seq(arr, left, j, bit - 1);
        if (i < right) qs_bin_seq(arr, i, right, bit - 1);
    }
}

void qs_bin_parallel(elem_t *arr, long n, int bit) {
    dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    dispatch_group_t group = dispatch_group_create();
    int bits = (int)(sizeof arr[0] * 8);
    if (bit >= bits) bit = bits - 1;
    qs_bin_par(arr, 0, n, bit, queue, group);
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
    // dispatch_release(group); // optional on older APIs
}

void qs_bin_sequential(elem_t *arr, long n, int bit){
    int bits = (int)(sizeof arr[0] * 8);
    if (bit >= bits) bit = bits - 1;
    if (bit < 0) return;
    qs_bin_seq(arr, 0, n, bit);
}
