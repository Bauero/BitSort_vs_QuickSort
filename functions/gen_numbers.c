#include <stdlib.h>

typedef unsigned long elem_t;

void generuj_liczby_losowe(elem_t *arr, size_t n) {
    for (size_t i = 0; i < n; i++)
        arr[i] = arc4random();
}
