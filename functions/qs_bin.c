#include <stdlib.h>
#include <dispatch/dispatch.h>
#include "__init__.h"


// static inline int initial_data_filter(elem_t* arr, size_t size) {
//     int s = sizeof(elem_t);
//     int bit = (s << 3) - 1;
    
//     elem_t greatest_in_array = 0;
//     elem_t max_value = 1ULL << bit;

//     for (int n = 0; n < size;){
//         if (greatest_in_array < arr[n]){
//             greatest_in_array = arr[n];
//         }
//         if (max_value <= greatest_in_array) break;
//     }

//     int counter = 0;
//     while (greatest_in_array > 0) {
//         greatest_in_array >>= 1;
//         ++counter;
//     }
//     return counter;
    
//     // for (long n = 0; n < size;) {
//     //     all_values_orred |= arr[n];
//     //     ++n;
//     //     // every n'th (n == size) iteration check if all_values_orred already
//     //     // have msb = 1 -> so we already know some numbers uses all bits
//     //     // and we can finish optimization
//     //     if (!(n & bit) && (all_values_orred & max_value)) break;
//     // }

//     // // Return top bit which could be used to divide data
//     // if (all_values_orred >= max_value){
//     //     return bit;
//     // } else {
//     //     int counter = 0;
//     //     while ((max_value > 0) && (max_value > all_values_orred)) {
//     //         max_value >>= 1;
//     //         ++counter;
//     //     }
//     //     return counter;
//     // }

// }

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
    if (left >= right || bit < 0) return;

    elem_t mask = 1;
    mask <<= bit;
    elem_t i = left, j = right;

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
    if (right - left) {
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

void qs_bin_parallel(elem_t *arr, long n) {
    dispatch_queue_t queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    dispatch_group_t group = dispatch_group_create();
    // int bit = initial_data_filter(arr, n);
    int bit = (int)(sizeof(elem_t) << 3) - 1;
    qs_bin_par(arr, 0, n, bit, queue, group);
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
}

void qs_bin_sequential(elem_t *arr, long n){
    // int bit = initial_data_filter(arr, n);
    int bit = (int)(sizeof(elem_t) << 3) - 1;
    // printf("n = %ld, bit = %ld\n", n-1, bit);
    qs_bin_seq(arr, 0, n, bit);
}
