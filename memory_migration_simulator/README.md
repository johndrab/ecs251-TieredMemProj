# Memory Management Simulation with Migration Policies

This project simulates memory management using different migration policies for memory pages between DRAM (Dynamic Random Access Memory) and NVM (Non-Volatile Memory). It evaluates the effectiveness of various migration strategies using memory access patterns.

## Features
- Simulated memory management using DRAM and NVM.
- Implementation of four different memory migration policies:
  - **No Migration**
  - **Threshold-Based Migration**
  - **Prefetching Migration**
  - **LRU (Least Recently Used) Migration**
- Generates memory access patterns to test the performance of each policy.
- Visualizes results using matplotlib for comparison.

## Requirements
- Python 3.x
- matplotlib

Install dependencies using:
```bash
pip install matplotlib
```

## Classes Overview
- `MigrationController`: Base class implementing threshold-based migration.
- `No_migration`: No memory migration is applied.
- `PrefetchingMigrationController`: Migrates adjacent pages if a page crosses the threshold.
- `LRUMigrationController`: Migrates the least recently used pages.

## Functions Overview
- `generate_memory_accesses()`: Generates sequential memory access patterns.
- `generate_random_memory_accesses()`: Generates random memory access patterns.
- `generate_LRU_memory_accesses()`: Simulates LRU-specific memory access patterns.
- `run_test()`: Runs a policy simulation and collects results.
- `plot_results()`: Visualizes the results of memory migrations and access time.

## How to Run
1. Clone the repository.
2. Run the following command:
```bash
python script_name.py
```
3. View the results in the console and the generated plots.

## Example Output
- Total Migrations
- Execution Time (ms)
- Memory Access Statistics (NVM vs DRAM)

## Explanation of Policies
- **No Migration:** Pages remain in their original memory location.
- **Threshold-Based Migration:** Pages are moved to DRAM when accessed more than a threshold value.
- **Prefetching Migration:** Neighboring pages are migrated with the accessed page to optimize future access.
- **LRU Migration:** Least recently used pages are migrated to NVM to free up space in DRAM.

## License
This project is licensed under the MIT License.

## Contributions
Contributions and suggestions are welcome! Feel free to submit a pull request or open an issue.
