#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/* Read the CPU’s time stamp counter */
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

/* Test 1: Sequential Memory Access with Different Strides */
void test_sequential_access(size_t arraySize, int minStride, int maxStride) {
    char *array = malloc(arraySize);
    if (!array) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }
    
    /* Initialize the array */
    for (size_t i = 0; i < arraySize; i++) {
        array[i] = (char)(i % 256);
    }

    printf("=== Sequential Access Test ===\n");
    for (int stride = minStride; stride <= maxStride; stride *= 2) {
        volatile char tmp;  // volatile to prevent optimization
        size_t accesses = 0;
        uint64_t start = rdtsc();
        for (size_t i = 0; i < arraySize; i += stride) {
            tmp = array[i];
            accesses++;
        }
        uint64_t end = rdtsc();
        double cyclesPerAccess = (double)(end - start) / accesses;
        printf("Stride: %4d bytes, Average cycles per access: %.2f\n", stride, cyclesPerAccess);
    }
    free(array);
}

/* Test 2: Random Pointer-Chasing */
void test_random_pointer_chasing(size_t numElements) {
    /* Allocate an array to hold a permutation of indices */
    size_t *indices = malloc(numElements * sizeof(size_t));
    if (!indices) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    /* Initialize with sequential indices */
    for (size_t i = 0; i < numElements; i++) {
        indices[i] = i;
    }

    /* Shuffle using Fisher-Yates */
    for (size_t i = numElements - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        size_t tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    /* Perform pointer chasing along the random chain */
    size_t index = 0;
    uint64_t start = rdtsc();
    for (size_t i = 0; i < numElements; i++) {
        index = indices[index];
    }
    uint64_t end = rdtsc();
    double cyclesPerAccess = (double)(end - start) / numElements;
    printf("=== Random Pointer-Chasing Test ===\n");
    printf("Average cycles per access: %.2f\n", cyclesPerAccess);
    
    free(indices);
}

int main(void) {
    srand(time(NULL));
    
    /* Test 1: Sequential access on a 64MB array */
    size_t arraySize = 64 * 1024 * 1024; // 64 MB
    int minStride = 4;   // 4 bytes (typical for an int)
    int maxStride = 1024; // up to 1024 bytes
    test_sequential_access(arraySize, minStride, maxStride);
    
    /* Test 2: Random pointer chasing on a large index array */
    size_t numElements = 8 * 1024 * 1024; // e.g., 8 million elements
    test_random_pointer_chasing(numElements);
    
    return 0;
}

