#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "zlib.h" // your Z_Heap header

#define N 100000

double now() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main() {
    printf("Benchmark: %d malloc/realloc operations\n", N);

    void *raw_ptrs[N];
    Z_Heap_Auto heap = {0};
    void *heap_ptrs[N];

    // --------------------------
    // 1. Raw malloc/realloc
    // --------------------------
    double t0 = now();
    for (int i = 0; i < N; i++) {
        raw_ptrs[i] = malloc(64);
    }
    for (int i = 0; i < N; i++) {
        raw_ptrs[i] = realloc(raw_ptrs[i], 128);
    }
    for (int i = 0; i < N; i++) {
        free(raw_ptrs[i]);
    }
    double t1 = now();
    printf("Raw malloc/realloc/free: %.6f sec\n", t1 - t0);

    // --------------------------
    // 2. Heap malloc/realloc
    // --------------------------
    double t2 = now();
    for (int i = 0; i < N; i++) {
        heap_ptrs[i] = z_heap_malloc(&heap, 64);
    }
    for (int i = 0; i < N; i++) {
        heap_ptrs[i] = z_heap_realloc(&heap, heap_ptrs[i], 128);
    }
    double t3 = now();
    printf("Z_Heap malloc/realloc/free: %.6f sec\n", t3 - t2);

    return 0;
}
