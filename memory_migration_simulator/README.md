Memory Management Simulation with Migration Policies

This project simulates memory management using different migration policies for memory pages between DRAM (Dynamic Random Access Memory) and NVM (Non-Volatile Memory). It evaluates the effectiveness of various migration strategies using memory access patterns.

Features:
Simulated memory management using DRAM and NVM.
Implementation of four different memory migration policies:
1. No Migration
2. Threshold-Based Migration
3. Prefetching Migration
4. LRU (Least Recently Used) Migration
Generates memory access patterns to test the performance of each policy.
Visualizes results using matplotlib for comparison.

Requirements
Python 3.x
matplotlib

Classes Overview
MigrationController: Base class implementing threshold-based migration.
No_migration: No memory migration is applied.
PrefetchingMigrationController: Migrates adjacent pages if a page crosses the threshold.
LRUMigrationController: Migrates the least recently used pages.

Functions Overview
generate_memory_accesses(): Generates sequential memory access patterns.
generate_random_memory_accesses(): Generates random memory access patterns.
generate_LRU_memory_accesses(): Simulates LRU-specific memory access patterns.
run_test(): Runs a policy simulation and collects results.
plot_results(): Visualizes the results of memory migrations and access time.

How to Run?
Clone the repository.
Run the following command:
View the results in the console and the generated plots.

Example Output:
Total Migrations
Execution Time (ms)
Memory Access Statistics (NVM vs DRAM)

Explanation of Policies
No Migration: Pages remain in their original memory location.
Threshold-Based Migration: Pages are moved to DRAM when accessed more than a threshold value.
Prefetching Migration: Neighboring pages are migrated with the accessed page to optimize future access.
LRU Migration: Least recently used pages are migrated to NVM to free up space in DRAM. *Note: the LRU page is a hardcode "85" in this script.
