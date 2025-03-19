#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <unistd.h>
#include <sys/mman.h>
#include <cassert>

#define PAGE_SIZE 4096  // Assuming 4 KB pages, can be adjusted based on system

// Function to simulate memory access and generate page faults
void memoryAccessTest(size_t arraySize, size_t numIterations, bool sequential = false) {
    void* ptr = mmap(NULL, arraySize * sizeof(int), PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Memory mapping failed! Error: " << strerror(errno) << std::endl;
        return;
    }

    std::vector<int> largeArray(static_cast<int*>(ptr), static_cast<int*>(ptr) + arraySize);
    size_t accesses = 0;

    // Random number generator for randomized access patterns
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, arraySize - 1);

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < numIterations; ++i) {
        size_t index;
        if (sequential) {
            index = i % arraySize;  // Sequential access pattern
        } else {
            index = dist(gen);  // Random access pattern
        }

        largeArray[index] = largeArray[index] + 1;
        accesses++;

        if (i % (numIterations / 10) == 0) {
            std::cout << "Progress: " << (i * 100 / numIterations) << "%" << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Completed " << accesses << " memory accesses in "
              << duration.count() << " seconds." << std::endl;
}

// Function to launch multiple memory stress tests
void runMultipleStressTests() {
    // Sizes in GB (converted to bytes and divided by sizeof(int))
    std::vector<size_t> memorySizes = { 25L, 50L, 75L, 100L };  // Array sizes for each sub-program (25GB, 50GB, 75GB, 100GB)
    
    size_t numIterations = 10L * 1024L * 1024L * 1024L / sizeof(int);  // 10 times the size of each array

    std::vector<std::thread> threads;

    for (size_t i = 0; i < memorySizes.size(); ++i) {
        size_t arraySize = memorySizes[i] * 1024L * 1024L * 1024L / sizeof(int);  // Convert GB to number of ints
        std::cout << "Launching memory stress test for " << memorySizes[i] << " GB..." << std::endl;

        threads.push_back(std::thread(memoryAccessTest, arraySize, numIterations, false));  // Random access
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    std::cout << "Starting memory stress tests..." << std::endl;
    runMultipleStressTests();
    return 0;
}

