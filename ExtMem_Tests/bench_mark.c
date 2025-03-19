#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE (1024 * 1024 * 512)  // 512MB 
#define NUM_ACCESSES 10000000         

int main() {
    int *array = (int *)malloc(ARRAY_SIZE * sizeof(int));
    if (array == NULL) {
        perror("malloc");
        return 1;
    }

    srand(time(NULL)); //random num gen

    // This will access array randomly 
    for (int i = 0; i < NUM_ACCESSES; i++) {
        int idx = rand() % ARRAY_SIZE;  
        array[idx] = rand(); //Performs write with random value
    }

    // Just in case im adding this to prevent the compiler from optimizing out the array
    volatile int unused = array[0];
    printf("Random accesses completed. Unused: %d\n", unused);

    free(array);
    return 0;
}

