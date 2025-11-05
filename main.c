#include <stdio.h>
#include <stdlib.h>

#define NUMBERS_LIMIT 10
#define DEBUG 1
#define TRACE 1

static long SORTED_1[NUMBERS_LIMIT];
static int sorted_counter = 0;

void generuj_liczby_losowe(long *array, int size);
void quick_sort(long *array, int length, int iter);

int main() {
    
    long losowe[NUMBERS_LIMIT];
    generuj_liczby_losowe(losowe, NUMBERS_LIMIT);
    
    if (TRACE) {
        printf("Liczby wylosowane:\n");
        for (int i = 0; i < NUMBERS_LIMIT; i++){
            printf("%ld", losowe[i]);
            (i < NUMBERS_LIMIT) ? printf(", ") : printf("\n");
        }
    }
    quick_sort(losowe, NUMBERS_LIMIT, ((int)sizeof(long)) * 8);
    
    if (TRACE) {
        printf("\nLiczby posortowane - SORTED_1:\n");
        for (int i = 0; i < NUMBERS_LIMIT; i++){
            printf("%ld", SORTED_1[i]);
            (i < NUMBERS_LIMIT) ? printf(", ") : printf("\n");
        }
    }

    for (int i = 0; i < NUMBERS_LIMIT - 1 ; i++)
    {
        if (! (SORTED_1[i] <= SORTED_1[i+1])) {
            printf("\nZnaleziono nierówność ! : %ld nie jest mniejsze równe %ld", SORTED_1[i], SORTED_1[i+1]);
            return 1;
        }
    }
    printf("Wszystko dobrze posortowane :)");
    return 0;
}

void quick_sort(long *array, int length, int iter) {
    
    if (DEBUG) {
        printf("\nStarting quicksort with iter = %d, length = %d and numbers: ", iter, length);
        for (int i = 0; i < length; i++){
            printf("%ld, ", array[i]);
        }
        printf("\n");
    }
    
    if (length == 0 ) {return; }
    if (iter < 0) { return; }
    
    long power = 1 << iter;
    long smaller_counter = 0;
    long greater_counter = 0;
    long first_smaller = 0;
    int smaller_anded = 1;
    long first_greater = 0;
    int greater_anded = 1;
    
    for (int i = 0; i < length; i++) {
        if ((array[i] & power) == 0)
        { 
            if (smaller_counter == 0){
                first_smaller = array[i];
            }
            smaller_anded = (first_smaller == array[i]) * smaller_anded;
            ++smaller_counter;
            
        }
        else {
            if (greater_counter == 0){
                first_greater = array[i];
            }
            greater_anded = (first_greater == array[i]) * greater_anded;
            ++greater_counter;
        }
    }
    
    if (DEBUG) {
        printf("First smaller: %ld, smaller anded: %d\n", first_smaller, smaller_anded);
        printf("First greater: %ld, greater anded: %d\n", first_greater, greater_anded);
    }

    if (smaller_counter == 1) {
        if (DEBUG) {
            printf("Iteration %d - smaller: %ld - adding value to sorted numbers\n", iter, first_smaller);
            printf("Adding %ld\n", first_smaller);
        }
        SORTED_1[sorted_counter++] = first_smaller;
    }
    else if (smaller_counter > 1) {
        if (smaller_anded == 1) {
            if (DEBUG) {
                printf("All numbers from this iteration are added to list\n");
            }
            for (int i = 0; i < smaller_counter; i++){
                printf("Adding %ld\n", array[i]);
                SORTED_1[sorted_counter++] = first_smaller;
            }
        }
        else {
            if (DEBUG) { printf("Iteration %d - smaller: ", iter); }
            long *smaller = (long *)calloc(smaller_counter, sizeof(long));
            long small_tab_counter = 0;
            for (int i = 0; i < length; i++) {
                if ((array[i] & power) == 0) {
                    smaller[small_tab_counter++] = array[i];
                    if (DEBUG) { printf("%ld, ", array[i]); }
                }
            }
            if (DEBUG) { printf("\n"); }
            quick_sort(smaller, smaller_counter, iter - 1);
            free(smaller);
        }
    }
    
    if (greater_counter == 1) {
        if (DEBUG) { printf("Iteration %d - greater: %ld\n", iter, first_greater); }
        if (DEBUG) { printf("Adding %ld\n", first_greater); }
        SORTED_1[sorted_counter++] = first_greater;
    }
    else if (greater_counter > 1)
    {
        if ( greater_anded == 1 ) {
            if (DEBUG) { printf("All numbers from this iteration are added to list\n"); }
            for (int i = 0; i < greater_counter; i++){
                if (DEBUG) { printf("Adding %ld\n", array[i]); }
                SORTED_1[sorted_counter++] = first_greater;
            }
        }
        else {
            if (DEBUG) { printf("Iteration %d - greater: ", iter); }
            long *greater = (long *)calloc(greater_counter, sizeof(long));
            long great_tab_counter = 0;
            for (int i = 0; i < length; i++) {
                if ((array[i] & power) != 0) {
                    greater[great_tab_counter++] = array[i];
                    if (DEBUG) { printf("%ld, ", array[i]); }
                }
            }
            if (DEBUG) { printf("\n"); }
            quick_sort(greater, greater_counter, iter - 1);
            free(greater);
        }
    }
}

void generuj_liczby_losowe(long *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = arc4random();
    }
}

