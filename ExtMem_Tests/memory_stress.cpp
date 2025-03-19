#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <cassert>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>  
#include <fstream>      
#define PAGE_SIZE 4096 

void memoryAccessTest(size_t arraySize, size_t numIterations) {
    void* ptr = mmap(NULL, arraySize * sizeof(int), PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Memory mapping failed! Error: " << strerror(errno) << std::endl;
        return;
    }

    std::vector<int> largeArray(static_cast<int*>(ptr), static_cast<int*>(ptr) + arraySize);
    size_t accesses = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, arraySize - 1);

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < numIterations; ++i) {
        size_t index = dist(gen); 
        largeArray[index] = largeArray[index] + 1;  // Writing to memory, avoiding optimizatio
        volatile int dummy = largeArray[index];  
        (void)dummy;  

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


void fileWriteTest(size_t numFiles, size_t fileSize, size_t numIterations) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, fileSize - 1);

    // Open multiple files for writing
    std::vector<std::ofstream> files;
    for (size_t i = 0; i < numFiles; ++i) {
        files.push_back(std::ofstream("file_" + std::to_string(i) + ".txt", std::ios::out | std::ios::binary));
        if (!files[i].is_open()) {
            std::cerr << "Error opening file " << i << std::endl;
            return;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    size_t writes = 0;

    for (size_t i = 0; i < numIterations; ++i) {
        // Choose a random file to write to
        size_t fileIndex = dist(gen) % numFiles;
        
        files[fileIndex].seekp(dist(gen), std::ios::beg);  // Seek to a random position in the file
        char buffer[1024];
        std::memset(buffer, 'A', sizeof(buffer));  // Fill the buffer with dummy data
        files[fileIndex].write(buffer, sizeof(buffer));  // Write to the file

        writes++;

        if (i % (numIterations / 10) == 0) {
            std::cout << "File write progress: " << (i * 100 / numIterations) << "%" << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Completed " << writes << " file writes in "
              << duration.count() << " seconds." << std::endl;

    for (auto& file : files) {
        file.close();
    }
}


void stressTest(size_t numThreads, size_t totalMemorySize, size_t numFiles, size_t fileSize, size_t timeoutSeconds) {
    size_t perThreadSize = totalMemorySize / numThreads;

    size_t numIterations = 10L * 1024L * 1024L * 1024L / sizeof(int);  // Adjust iterations based on the allocated memory size

    std::vector<std::thread> threads;

    // Start threads
    for (size_t i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(memoryAccessTest, perThreadSize, numIterations));
    }

    std::thread fileWriteThread(fileWriteTest, numFiles, fileSize, numIterations);
    std::this_thread::sleep_for(std::chrono::seconds(timeoutSeconds));
    for (auto& t : threads) {
        t.join();
    }
    fileWriteThread.join();

    std::cout << "Stress test completed" << std::endl;
}

int main() {
    // Parameters
    size_t numThreads = 10;            // Number of virtual memory workers
    size_t totalMemorySize = 20L * 1024L * 1024L * 1024L / sizeof(int);  //20GB
    size_t numFiles = 5;               // Number of files to write to
    size_t fileSize = 5L * 1024L * 1024L * 1024L;
    size_t timeoutSeconds = 30;        // Run the stress test for 30 seconds

    setenv("DRAMSIZE", "10G", 1);      // Limits DRAM to 10 GB
    setenv("SWAPDIR", "/dev/sda99", 1);  // Set swap file path, for Cloudsystems so far i've seen mostly /dev/sda99

    std::cout << "Starting stress test with ExtMem..." << std::endl;
    stressTest(numThreads, totalMemorySize, numFiles, fileSize, timeoutSeconds);

    return 0;
}

