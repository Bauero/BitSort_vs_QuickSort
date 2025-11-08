#include <stdio.h>
#include <stdlib.h>

#define NUMBERS_LIMIT 10
#define DEBUG 1
#define TRACE 1

void bit_quicksort(unsigned long* array, int left, int right, int bit) {
    if (left >= right || bit < 0)
        return;

    if (DEBUG) {
        printf("\n[bit %d] Zakres [%d, %d]: ", bit, left, right);
        for (int i = left; i <= right; i++) printf("%lu ", array[i]);
        printf("\n");
    }

    unsigned long mask = 1UL << bit;
    int i = left;
    int j = right;

    // Podział: 0 po lewej, 1 po prawej
    while (i <= j) {
        while (i <= right && (array[i] & mask) == 0) i++;
        while (j >= left && (array[j] & mask) != 0) j--;

        if (i < j) {
            unsigned long tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
            i++;
            j--;
        }
    }
    
    bit_quicksort(array, left, j, bit - 1);   // lewa część (bit = 0)
    bit_quicksort(array, i, right, bit - 1);  // prawa część (bit = 1)
}

void generuj_liczby_losowe(long *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = arc4random();
    }
}

