import multiprocessing as mp
import time
import os

def allocate_memory(size_in_gb, duration):
    """Allocate a specified amount of memory for a given duration."""
    size_in_bytes = size_in_gb * 1024 * 1024 * 1024  # Convert GB to bytes
    print(f"Process {os.getpid()} allocating {size_in_gb}GB of memory...")
    try:
        # Allocate memory by creating a large bytearray
        memory_block = bytearray(size_in_bytes)
        for i in range(0, len(memory_block), 4096):  # Touch pages to ensure they are allocated
            memory_block[i] = 1
        print(f"Process {os.getpid()} allocated {size_in_gb}GB of memory.")
        time.sleep(duration)  # Keep memory allocated for the duration
    except MemoryError:
        print(f"Process {os.getpid()} failed to allocate memory.")
    finally:
        print(f"Process {os.getpid()} releasing memory.")


def stress_memory(num_workers, size_per_worker, duration):
    """Spawn multiple processes to allocate memory."""
    processes = []
    for _ in range(num_workers):
        p = mp.Process(target=allocate_memory, args=(size_per_worker, duration))
        p.start()
        processes.append(p)

    for p in processes:
        p.join()


if __name__ == "__main__":
    NUM_WORKERS = 10       # Equivalent to --vm 10
    SIZE_PER_WORKER = 2    # 20GB total, divided by 10 workers (20G / 10 = 2G each)
    DURATION = 30          # 30 seconds timeout
    
    print("Starting memory stress test...")
    stress_memory(NUM_WORKERS, SIZE_PER_WORKER, DURATION)
    print("Memory stress test completed.")

