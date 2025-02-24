#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

int main() {
    size_t pagesize = sysconf(_SC_PAGESIZE);

    // Allocate memory aligned to the page size
    void *addr = aligned_alloc(pagesize, pagesize);
    if (addr == NULL) {
        perror("aligned_alloc");
        return 1;
    }

    // Print the allocated memory address
    printf("Allocated memory address: %p\n", addr);

    // Measure time for mprotect
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (mprotect(addr, pagesize, PROT_READ | PROT_WRITE) == -1) {
        perror("mprotect");
        free(addr);
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time for mprotect: %f seconds\n", elapsed);

    // Clean up
    free(addr);
    return 0;
}



