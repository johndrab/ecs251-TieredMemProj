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
    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    memset(addr, 1, size);
    return addr;
}

void apply_extmem_policy(const char *policy, pid_t pid, void *addr) {
    char command[256];
    snprintf(command, sizeof(command), "echo %d %p > /sys/kernel/extmem/%s", pid, addr, policy);
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Warning: system command '%s' failed with return code %d\n", command, ret);
    }
}

void apply_prefetch(void *addr, size_t size, pid_t pid) {
    printf("Applying ExtMem prefetching...\n");
    apply_extmem_policy("prefetch", pid, addr);
}

void apply_thrashing(size_t size, pid_t pid) {
    printf("Applying ExtMem thrashing...\n");
    apply_extmem_policy("thrash", pid, NULL);
}

void *apply_batch_remap(void *addr, size_t size, pid_t pid) {
    printf("Applying ExtMem batch remapping...\n");
    apply_extmem_policy("migrate", pid, addr);
    return addr;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <use_swap (0 or 1)> <memory_size_in_MB>\n", argv[0]);
        return 1;
    }

    int use_swap = atoi(argv[1]);
    size_t memory_size = SIZE_MB(atoll(argv[2]));
    pid_t pid = getpid();

    void *src = allocate_memory(memory_size, use_swap);
    void *dst = allocate_memory(memory_size, use_swap);

    apply_prefetch(src, memory_size, pid);
    apply_prefetch(dst, memory_size, pid);
    apply_thrashing(memory_size, pid);
    src = apply_batch_remap(src, memory_size, pid);
    dst = apply_batch_remap(dst, memory_size, pid);

    measure_memcpy(src, dst, memory_size);
    munmap(src, memory_size);
    munmap(dst, memory_size);

    return 0;
}
