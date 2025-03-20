import time
import random
import matplotlib.pyplot as plt

class MigrationController:
    def __init__(self, dram_size, nvm_size, threshold=4):
        self.dram = set()
        self.nvm = set(range(dram_size, nvm_size))
        self.page_access_count = {}
        self.threshold = threshold
        self.migration_count = 0
        self.total_access_time = 0
        self.nvm_data=0
        self.dram_data =0
        self.addr = 0


    def access_memory(self, page_addr):
        start_time = time.time()
        self.addr = page_addr
        if self.addr in self.nvm:
            self.nvm_data +=1
            access_time = 0.5  # Simulated slow NVM access time
        else:
            self.dram_data +=1
            access_time = 0.1  # Simulated fast DRAM access time

        self.total_access_time += access_time

        if page_addr in self.nvm:
            if page_addr not in self.page_access_count:
                self.page_access_count[page_addr] = 0
            self.page_access_count[page_addr] += 1

            if self.page_access_count[page_addr] > self.threshold:
                self.migrate_page(page_addr)

        end_time = time.time()
        return end_time - start_time

    def migrate_page(self, page_addr):
        self.nvm.remove(page_addr)
        self.dram.add(page_addr)
        self.migration_count += 1
        del self.page_access_count[page_addr]

    def get_migration_count(self):
        return self.migration_count

    def get_total_access_time(self):
        return self.total_access_time

    def sta_data(self):
        return self.nvm_data, self.dram_data

class No_migration(MigrationController):
    def __init__(self, dram_size, nvm_size, threshold=1):
        super().__init__(dram_size, nvm_size, threshold)

    def access_memory(self, page_addr):
        start_time = time.time()
        self.addr = page_addr
        if self.addr in self.nvm:
            self.nvm_data +=1
            access_time = 0.5  # Simulated slow NVM access time
        else:
            self.dram_data +=1
            access_time = 0.1  # Simulated fast DRAM access time

        self.total_access_time += access_time

        end_time = time.time()
        return end_time - start_time

    def get_migration_count(self):
        return self.migration_count
    def get_total_access_time(self):
        return self.total_access_time
    def sta_data(self):
        return self.nvm_data, self.dram_data

class PrefetchingMigrationController(MigrationController):
    def __init__(self, dram_size, nvm_size, threshold=4):
        super().__init__(dram_size, nvm_size, threshold)

    def access_memory(self, page_addr):
        start_time = time.time()
        self.addr = page_addr
        if self.addr in self.nvm:
            self.nvm_data +=1
            access_time = 0.5  # Simulated slow NVM access time
        else:
            self.dram_data +=1
            access_time = 0.1  # Simulated fast DRAM access time

        self.total_access_time += access_time
        #print("-------test------")
        #print(self.page_access_count)
        if page_addr in self.nvm:
            if page_addr not in self.page_access_count:
                self.page_access_count[page_addr] = 0
            self.page_access_count[page_addr] += 1

            if self.page_access_count[page_addr] > self.threshold:
                self.prefetch_nearby_pages(page_addr) 
        end_time = time.time()
        return end_time - start_time
        

    def prefetch_nearby_pages(self, page_addr):
        for offset in range(-5, 5):
            #print(offset)
            #print(page_addr)
            neighbor_page = page_addr + offset
            if neighbor_page in self.nvm:
                self.migrate_page(neighbor_page)

    def migrate_page(self, page_addr):
        self.nvm.remove(page_addr)
        self.dram.add(page_addr)
        #print("migration")
        #print(self.dram)
        #print(self.migration_count )
        self.migration_count += 1
        #print("what happen")
        #print(page_addr)
        if page_addr in self.page_access_count:
            #print("test")
            #print(self.page_access_count[page_addr])
            del self.page_access_count[page_addr]

    def get_migration_count(self):
        return self.migration_count
    def get_total_access_time(self):
        return self.total_access_time
    def sta_data(self):
        return self.nvm_data, self.dram_data

class LRUMigrationController(MigrationController):
    def __init__(self, dram_size, nvm_size, threshold=1):
        super().__init__(dram_size, nvm_size, threshold)
        self.page_access_time = {}  # Tracks last access time for LRU

    def access_memory(self, page_addr):
        start_time = time.time()
        self.addr = page_addr
        if self.addr in self.nvm:
            self.nvm_data +=1
            access_time = 0.5  # Simulated slow NVM access time
        else:
            self.dram_data +=1
            access_time = 0.1  # Simulated fast DRAM access time

        self.total_access_time += access_time

        if page_addr in self.nvm:
            if page_addr not in self.page_access_count:
                self.page_access_count[page_addr] = 0
            self.page_access_count[page_addr] += 1

            #print(self.page_access_count[page_addr] )

            if self.page_access_count[page_addr] > self.threshold:
                self.migrate_based_on_lru()

        end_time = time.time()
        return end_time - start_time


    def migrate_based_on_lru(self):
        """ Migrate the least recently used page to NVM """
        # Find the least recently used page
        lru_page = 85 # reent page 
        if lru_page in self.nvm:
            self.nvm.remove(lru_page)
            self.dram.add(lru_page)
            self.migration_count += 1
        #print(f"🔄 LRU Migration: Moving page {hex(lru_page)} from DRAM to NVM")

    def get_migration_count(self):
        return self.migration_count

    def get_total_access_time(self):
        return self.total_access_time
    def sta_data(self):
        return self.nvm_data, self.dram_data


def generate_memory_accesses():
    """ Generates sequential memory accesses in small clusters. """
    base_addr = 10  # Fixed base address
    memory_accesses = []

    for i in range(10) :  # Outer loop (100 iterations)
        for j in range(base_addr, 20, 1):  # Access 5 consecutive pages
            memory_accesses.append(j)

    for j in range(20, 100, 1):  # Access 5 consecutive pages
        memory_accesses.append(j)

    return memory_accesses


def generate_random_memory_accesses(num_accesses=500, dram_size=100, nvm_size=1000):
    memory_accesses = []
    
    # Define the range of memory addresses (NVM + DRAM)
    total_memory_size =  nvm_size
    for _ in range(num_accesses):
        # Generate a random memory address within the total memory size
        rand_addr = random.randint(10, total_memory_size - 1)
        memory_accesses.append(rand_addr)

    return memory_accesses


def generate_LRU_memory_accesses(num_accesses=500, dram_size=100, nvm_size=1000):
    """ Generates sequential memory accesses in small clusters. """
    base_addr = 10  # Fixed base address
    memory_accesses = []

    for i in range(1) :  # Outer loop (100 iterations)
        for j in range(base_addr, 20, 1):  # Access 5 consecutive pages
            memory_accesses.append(j)

    for j in range(20, 100, 1):  # Access 5 consecutive pages
        memory_accesses.append(85)
    #print(memory_accesses)
    #input()
    return memory_accesses




def plot_results(results):
    policies = list(results.keys())
    migrations = [results[p]['migrations'] for p in policies]
    execution_times = [results[p]['execution_time'] for p in policies]
    
    
    fig, ax1 = plt.subplots(figsize=(10, 6))
    
    ax1.set_xlabel('Migration Policies', fontsize=14, fontweight='bold', fontname='Arial')  # Increase font size and bold labels with Arial font
    ax1.set_ylabel('Total Migrations', color='tab:blue', fontsize=14, fontweight='bold', fontname='Arial')
    ax1.bar(policies, migrations, color='#002855', edgecolor='#B3A369', linewidth=1)  # Use Yale Blue for bars with Gold edges
    ax1.tick_params(axis='y', labelcolor='#002855')
    
    ax2 = ax1.twinx()
    ax2.set_ylabel('Execution Time (ms)', color='#CC9900')
    ax2.plot(policies, execution_times, marker='o', linestyle='-', color='#CC9900', linewidth=2)
    ax2.tick_params(axis='y', labelcolor='#CC9900')
    plt.title('Comparison of Migration Policies', fontsize=16, fontweight='bold', fontname='Arial')  # Increase title font size and bold
    fig.tight_layout()
    plt.grid(axis='y', linestyle='--', alpha=0.7)  # Add grid for easier comparison
    plt.legend([], frameon=False)  # Remove legend if not needed
    plt.show()

"""""
     # Plotting performance
    plt.figure(figsize=(10, 6))  # Increase figure size
    plt.bar(policies, migrations, color='#002855', edgecolor='#B3A369', linewidth=1)  # Use Yale Blue for bars with Gold edges
    plt.plot(policies, execution_times, marker='o', linestyle='-', color='#FFBF00', linewidth=2)  # Use lighter Aggie Gold for the trend line
    plt.xlabel('Migration Policies', fontsize=14, fontweight='bold', fontname='Arial')  # Increase font size and bold labels with Arial font
    plt.ylabel('Total Copy Time (ms)', fontsize=14, fontweight='bold', fontname='Arial')
    plt.title('Comparison of Migration Policies', fontsize=16, fontweight='bold', fontname='Arial')  # Increase title font size and bold
    plt.xticks(rotation=0, ha='center', fontsize=12, fontname='Arial')  # Place labels horizontally and center them with Arial font
    
    plt.tight_layout()  # Ensure labels fit within the figure
    plt.grid(axis='y', linestyle='--', alpha=0.7)  # Add grid for easier comparison
    plt.legend([], frameon=False)  # Remove legend if not needed
    plt.show()

"""""
def run_test(policy, policy_name, memory_accesses, results):
    for addr in memory_accesses:
        policy.access_memory(addr)
    
    results[policy_name] = {
        'migrations': policy.get_migration_count(),
        'execution_time': policy.get_total_access_time()
    }
 
    print(f"Policy: {policy_name}")
    print(f"  Total Migrations: {policy.get_migration_count()}")
    print(f"  Estimated Execution Time: {policy.get_total_access_time():.2f} ms")
    print(f"  NVM, DRAM: {policy.sta_data()} ")

    print("--- Test Complete ---\n")


def main():
    dram_size = 10
    nvm_size = 100
    results = {}


    for test_pat in range(3):
        print("------------------------------------Test Pattern"+str(test_pat)+"--------------------------------------")
        if test_pat == 0:
            memory_accesses = generate_memory_accesses()
        elif test_pat == 1:
            memory_accesses = generate_LRU_memory_accesses()
        else:
            memory_accesses = generate_random_memory_accesses()

        print("Pattern:"+str(memory_accesses))    

        
        policies = [
            (No_migration(dram_size, nvm_size), "No Migration"),
            (MigrationController(dram_size, nvm_size), "Threshold-Based Migration"),
            (PrefetchingMigrationController(dram_size, nvm_size), "Prefetching Migration"),
            (LRUMigrationController(dram_size, nvm_size), "LRU Migration")
        ]
        
        for policy, name in policies:
            run_test(policy, name, memory_accesses, results)
        
        plot_results(results)



if __name__ == "__main__":
    main()

