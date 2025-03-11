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

#define SIZE_MB(x) ((x) * 1024 * 1024)

void measure_memcpy(void *src, void *dst, size_t size) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    memcpy(dst, src, size);

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    printf("Memcpy Time: %.2f microseconds\n", elapsed);

    sleep(5);
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

    if (use_swap) {
        madvise(addr, size, MADV_PAGEOUT);  // Force swap placement
    }

    memset(addr, 1, size);  // Touch pages to allocate
    return addr;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <use_swap (0 or 1)> <memory_size_in_MB>\n", argv[0]);
        return 1;
    }

    int use_swap = atoi(argv[1]);
    size_t memory_size = SIZE_MB(atoll(argv[2]));  // Convert from MB to bytes

    printf("Allocating memory in %s of size %zu MB...\n", use_swap ? "Swap" : "DRAM", memory_size / (1024 * 1024));

    void *src = allocate_memory(memory_size, use_swap);
    void *dst = allocate_memory(memory_size, use_swap);

    printf("Testing memcpy in %s...\n", use_swap ? "Swap" : "DRAM");
    measure_memcpy(src, dst, memory_size);

    munmap(src, memory_size);
    munmap(dst, memory_size);

    return 0;
}
