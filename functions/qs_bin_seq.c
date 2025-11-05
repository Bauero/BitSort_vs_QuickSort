typedef unsigned long elem_t;

void gs_bin_seq(elem_t *arr, long left, long right, int bit) {
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
    gs_bin_seq(arr, left, j, bit - 1);
    gs_bin_seq(arr, i, right, bit - 1);
}
