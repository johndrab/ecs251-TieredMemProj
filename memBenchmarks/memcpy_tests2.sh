BENCHMARK="./memcpy_benchmark"
OUTPUT_DIR="./results"

mkdir -p $OUTPUT_DIR

SWAP_DEVICE=$(sudo swapon --show | awk 'NR==2 {print $1}')
if [[ -z "$SWAP_DEVICE" ]]; then
    echo "No active swap device found. Check swap settings."
    exit 1
fi

clear_memory() {
    echo "Clearing Swap and Page Cache..."
    sync
    echo 3 | sudo tee /proc/sys/vm/drop_caches > /dev/null
    sudo swapoff "$SWAP_DEVICE" && sudo swapon "$SWAP_DEVICE"
    sleep 2
}

log_memory_movement() {
    PID=$1
    sleep 2
    if [ -e /proc/$PID/numa_maps ]; then
        cat /proc/$PID/numa_maps > "$OUTPUT_DIR/memory_movement_$PID.txt"
    else
        echo "Warning: Process $PID exited before logging memory movement." > "$OUTPUT_DIR/memory_movement_$PID.txt"
    fi
}

run_test() {
    CONFIG=$1
    USE_SWAP=$2
    MEMORY_SIZE_MB=$3
    STRATEGY=$4
    STRATEGY_NAME=$5
    OUTPUT_FILE="$OUTPUT_DIR/memcpy_${CONFIG}_${STRATEGY_NAME}.txt"
    PERF_FILE="$OUTPUT_DIR/perf_${CONFIG}_${STRATEGY_NAME}.txt"

    echo "Running memcpy benchmark ($CONFIG - $STRATEGY_NAME)..."

    sudo $BENCHMARK $USE_SWAP $MEMORY_SIZE_MB $STRATEGY &> $OUTPUT_FILE &
    PID=$!
    sleep 1

    log_memory_movement $PID

    sudo perf stat -e page-faults,major-faults,pgscan_direct -p $PID &> "$PERF_FILE" &
    PERF_PID=$!

    wait $PID
    wait $PERF_PID

    echo "Memcpy $CONFIG - $STRATEGY_NAME completed. Results saved to $OUTPUT_DIR/"
}

DRAM_MEMORY_SIZE_MB=512
SWAP_MEMORY_SIZE_MB=8192

clear_memory
run_test "DRAM" 0 $DRAM_MEMORY_SIZE_MB 0 "none"
clear_memory
run_test "DRAM" 0 $DRAM_MEMORY_SIZE_MB 1 "prefetch"
clear_memory
run_test "DRAM" 0 $DRAM_MEMORY_SIZE_MB 2 "thrashing"
clear_memory
run_test "DRAM" 0 $DRAM_MEMORY_SIZE_MB 3 "batch_remap"

clear_memory
run_test "Swap" 1 $SWAP_MEMORY_SIZE_MB 0 "none"
clear_memory
run_test "Swap" 1 $SWAP_MEMORY_SIZE_MB 1 "prefetch"
clear_memory
run_test "Swap" 1 $SWAP_MEMORY_SIZE_MB 2 "thrashing"
clear_memory
run_test "Swap" 1 $SWAP_MEMORY_SIZE_MB 3 "batch_remap"

echo "All tests completed."
