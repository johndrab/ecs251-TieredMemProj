#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define SIZE_MB(x) ((x) * 1024 * 1024)
#define PAGE_SIZE 4096
#define THRASH_ITERATIONS 10

void measure_memcpy(void *src, void *dst, size_t size) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    memcpy(dst, src, size);

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    printf("Memcpy Time: %.2f microseconds\n", elapsed);
}

void *allocate_memory(size_t size, int use_swap) {
    void *addr;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;

    if (use_swap) {
        flags |= MAP_NORESERVE;
    }

    addr = mmap(NULL, size, PROT_READ | PROT_WRITE, flags, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    memset(addr, 1, size);  // Touch pages to allocate in DRAM
    return addr;
}

void force_swap_out(void *addr, size_t size) {
    if (madvise(addr, size, MADV_PAGEOUT) == -1) {
        perror("madvise PAGEOUT failed");
    } else {
        printf("Memory swapped out to disk.\n");
    }
}

void batch_remap_swap_to_dram(void *addr, size_t size) {
    printf("Remapping memory from SWAP to DRAM...\n");

    // Prefetch memory to DRAM
    if (madvise(addr, size, MADV_WILLNEED) == -1) {
        perror("madvise WILLNEED failed");
    }

    // Lock pages in RAM to prevent them from being swapped out again
    if (mlock(addr, size) == -1) {
        perror("mlock failed");
    } else {
        printf("Memory locked in DRAM.\n");
    }

    // Access pages to ensure migration
    for (size_t i = 0; i < size; i += PAGE_SIZE) {
        ((char *)addr)[i] ^= 1; // Touch pages to force actual load into RAM
    }
}

void induce_thrashing(void *addr, size_t size) {
    printf("Inducing thrashing...\n");
    for (int iter = 0; iter < THRASH_ITERATIONS; iter++) {
        for (size_t i = 0; i < size; i += PAGE_SIZE) {
            ((char *)addr)[i] ^= 1; // Random memory access to force page faults
        }
    }
    printf("Thrashing completed.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <use_swap (0 or 1)> <memory_size_in_MB>\n", argv[0]);
        return 1;
    }

    int use_swap = atoi(argv[1]);
    size_t memory_size = SIZE_MB(atoll(argv[2]));

    printf("Allocating memory in %s of size %zu MB...\n", use_swap ? "Swap" : "DRAM", memory_size / (1024 * 1024));
    void *src = allocate_memory(memory_size, use_swap);
    void *dst = allocate_memory(memory_size, use_swap);

    printf("Testing memcpy in %s before swapping...\n", use_swap ? "Swap" : "DRAM");
    measure_memcpy(src, dst, memory_size);

    printf("Forcing memory to swap out...\n");
    force_swap_out(src, memory_size);
    force_swap_out(dst, memory_size);

    printf("Batch remapping memory from SWAP to DRAM...\n");
    batch_remap_swap_to_dram(src, memory_size);
    batch_remap_swap_to_dram(dst, memory_size);

    printf("Inducing thrashing after remapping...\n");
    induce_thrashing(src, memory_size);
    induce_thrashing(dst, memory_size);

    printf("Testing memcpy after remapping and thrashing...\n");
    measure_memcpy(src, dst, memory_size);

    munmap(src, memory_size);
    munmap(dst, memory_size);
    return 0;
}
