# Test scripts Overview

## Main Tool: `memory_analyzer_v2.py`
This script is designed to analyze memory access patterns of a given target program using Linux's `perf` tool. It will collect performance metrics, parse the output, and then recommend a memory policy based on observed behaviors. Designed to be an extention of the high-level memory simulator to provide a quick way to profile a program and assist in assessing an appropriate memory policy.  

## Features
- Runs `perf` on a given target program to collect memory-related performance data.
- Parses the output of `perf` to extract cache misses, cache references, major faults, and minor faults.
- Analyzes the collected data to determine potential memory inefficiencies.
- Uses a weighted scoring system to recommend one of four memory migration policies:
  - **Prefetching Migration**
  - **LRU Migration**
  - **Threshold-Based Migration**
  - **No Migration**

## Installation
Ensure `perf` is installed on your system:
```sh
sudo apt install linux-perf
```
Then, download or clone the script and ensure you have Python 3 installed.

## Usage
Run the script with the target program as an argument:
```sh
python3 memory_analysis_v2.py --cache-miss-weight 0.4 --major-fault-weight 0.4 --minor-fault-weight 0.2 --prefetch-threshold 0.8 --lru-threshold 0.6 --threshold-based-threshold 0.4 /path/to/target_program
```

### Arguments
- `program`: Path to the target executable to analyze.
- `--cache-miss-weight`: Weight assigned to cache miss score (default: 0.4).
- `--major-fault-weight`: Weight assigned to major fault score (default: 0.4).
- `--minor-fault-weight`: Weight assigned to minor fault score (default: 0.2).
- `--prefetch-threshold`: Threshold for prefetching recommendation (default: 0.8).
- `--lru-threshold`: Threshold for LRU recommendation (default: 0.6).
- `--threshold-based-threshold`: Threshold for threshold-based recommendation (default: 0.4).

Assign higher weights to metrics which occur most significant cost for the type of operations the program with be running, i.e a program managling signifiant memory sizes would have a high major-fault weight as the number of page-faults will be an important metric to minimize as much as possible. 

## Script Breakdown

### `run_perf(target_program, output_file="perf_output.txt")`
- Runs `perf record` on the target program, collecting data on cache misses, cache references, and page faults.
- Saves the `perf report` output to `perf_output.txt`.

### `parse_perf_output(file="perf_output.txt")`
- Parses the output file to extract memory event counts.
- Returns a dictionary containing event counts.

### `analyze_memory_patterns(data, args)`
- Normalizes and scores cache misses, major faults, and minor faults.
- Computes a combined score based on user-defined weights.
- Determines a recommended memory migration policy based on threshold values.

## Example Output
```
Running memory analysis on: ./my_program
Perf data collected and saved.
Raw Data Values:
Cache Misses: 50000
Cache References: 200000
Major Faults: 30
Minor Faults: 1000

Calculated Scores:
Cache Miss Ratio: 0.25
Major Fault Ratio: 0.00
Minor Faults: 1000
Cache Miss Score: 0.25
Major Fault Score: 0.00
Minor Fault Score: 0.10

Combined Score: 0.22
Recommended Memory Policy: No Migration
```

### Notes
- The thresholds and weights are adjustable to fit different system characteristics.
- Default thresholds are adapted for large data processing, recommended to run with data sizes over 10GB


---

# Memory Profiling Scripts:

## `bench_mark.c`
This program performs random memory accesses on a dynamically allocated array to simulate unpredictable memory access patterns.

## Features
- Allocates a **512MB** integer array dynamically.
- Performs **10 million** random memory accesses.

## Compilation & Execution
```sh
gcc -o bench_mark bench_mark.c
./bench_mark
```

## Example Output
```
Random accesses completed. Unused: 123456
```

---

## `bench_markDC.c`
This program performs large-scale file I/O operations to test disk performance. It writes, reads, and executes random access operations on a **5GB** file.

## Features
- Writes **5GB** of random data to a file.
- Reads the entire file sequentially.
- Performs **1000** random read/write operations.
- Uses buffered I/O with **4KB** chunks.

## Compilation & Execution
```sh
gcc -o bench_markDC bench_markDC.c
./bench_markDC
```

## Example Output
```
Writing 5368709120 bytes to file large_file.dat...
Performing random read/write operations...
Reading data from file...
```

###  Notes
- The file `large_file.dat` will be created in the current directory.
- Adjust `FILE_SIZE` and `NUM_OPERATIONS` as needed.

---

## `file_processer.c`
Similar to bench_markDC, this program performs large-scale file I/O operations but on multiple specified files.

## Features
- Writes **5GB** of random data to each specified file.
- Reads the entire file sequentially.
- Performs **1000** random read/write operations per file.
- Uses buffered I/O with **4KB** chunks.

## Compilation & Execution
```sh
gcc -o file_processer file_processer.c
./file_processer file1.dat file2.dat
```

## Example Output
```
Processing file: file1.dat
Writing 5368709120 bytes to file file1.dat...
Performing random read/write operations...
Reading data from file...
Processing file: file2.dat
Writing 5368709120 bytes to file file2.dat...
Performing random read/write operations...
Reading data from file...
```

---

## `memory_stress.cpp`
This program performs intensive memory and file operations to simulate high-load scenarios.

## Features
- Memory Access Test: Random memory accesses using `mmap` for large arrays.
- File Write Test: Writes randomly to multiple files with buffered data.
- Multi-threaded Stress Test: Simultaneously runs memory and file operations across multiple threads.

## Compilation & Execution
```sh
g++ -o memory_stress memory_stress.cpp -pthread  
./memory_stress
```

### Notes
(Default parameters: 10 threads, 20GB memory, 5 files of 5GB each, 30s timeout)




