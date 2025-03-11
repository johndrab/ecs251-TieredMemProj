BENCHMARK="./memcpy_benchmark"
OUTPUT_DIR="./results"
NUMA_MAPS="/proc/self/numa_maps"

mkdir -p $OUTPUT_DIR
SWAP_DEVICE=$(sudo swapon --show | awk 'NR==2 {print $1}')
if [[ -z "$SWAP_DEVICE" ]]; then
    echo "No active swap device found. Check swap settings."
    exit 1
fi

# Function to clear swap & cache
clear_memory() {
    echo "Clearing Swap and Page Cache..."
    sync
    echo 3 | sudo tee /proc/sys/vm/drop_caches > /dev/null
    sudo swapoff "$SWAP_DEVICE" && sudo swapon "$SWAP_DEVICE"
    sleep 2
}

log_memory_movement() {
    PID=$1
    sleep 2  # Increase sleep to give the system time
    if [ -e /proc/$PID/numa_maps ]; then
        cat /proc/$PID/numa_maps > "$OUTPUT_DIR/memory_movement_$PID.txt"
    else
        echo "Warning: Process $PID exited before logging memory movement." > "$OUTPUT_DIR/memory_movement_$PID.txt"
    fi
}

# Function to run memcpy test
run_test() {
    CONFIG=$1
    USE_SWAP=$2
    MEMORY_SIZE_MB=$3  # Pass memory size as argument
    OUTPUT_FILE="$OUTPUT_DIR/memcpy_$CONFIG.txt"
    PERF_FILE="$OUTPUT_DIR/perf_$CONFIG.txt"

    echo "Running memcpy benchmark ($CONFIG)..."

    # Start the benchmark
    sudo $BENCHMARK $USE_SWAP $MEMORY_SIZE_MB &
    PID=$!
    sleep 1  # Allow startup

    # Monitor memory allocation
    log_memory_movement $PID

    # Monitor page faults, migrations, and memory access
    sudo perf stat -e page-faults,major-faults,pgscan_direct -p $PID &> "$PERF_FILE" &
    PERF_PID=$!

    # Wait for benchmark to complete
    wait $PID
    wait $PERF_PID

    echo "Memcpy $CONFIG completed. Results saved to $OUTPUT_DIR/"
}

DRAM_MEMORY_SIZE_MB=512 
SWAP_MEMORY_SIZE_MB=8192 

clear_memory
run_test "DRAM" 0 $DRAM_MEMORY_SIZE_MB  # Run memcpy test in DRAM
clear_memory
run_test "Swap" 1 $SWAP_MEMORY_SIZE_MB  # Run memcpy test in Swap

echo "All tests completed."
