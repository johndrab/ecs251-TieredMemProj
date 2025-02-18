#include <numa.h>
#include <iostream>
#include <chrono>
#include <cstring>

int main() {
    // can change to different data size to test memory access costs
    size_t size = 1024 * 1024 * 100; // 100 MB

    // Allocate both src and dst on NUMA node 0 (fast tier)
    char* src = (char*)numa_alloc_onnode(size, 0);
    char* dst = (char*)numa_alloc_onnode(size, 0);

    auto start = std::chrono::high_resolution_clock::now();
    std::memcpy(dst, src, size);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "memcpy time (fast tier -> fast tier): "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms" << std::endl;

    numa_free(src, size);
    numa_free(dst, size);
    return 0;
}
