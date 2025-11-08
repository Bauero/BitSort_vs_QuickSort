#ifndef __INIT___H
#define __INIT___H

typedef unsigned long elem_t;

// #define PARALLEL_THRESHOLD (elem_t)1 << (sizeof(elem_t) << 2)
#define PARALLEL_THRESHOLD 10000
#define SWAP(a, b) { elem_t tmp = (a); (a) = (b); (b) = tmp; }

#endif