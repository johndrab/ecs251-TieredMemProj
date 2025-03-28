# **Guide for Getting Perf to Work**

## **Install Commands:**
```sh
sudo apt-get update
sudo apt-get install linux-intel-iotg-5.15-tools-common
sudo apt-get install linux-tools-5.4.0-164-generic
sudo sysctl -w kernel.perf_event_paranoid=0
```

**Perf Basics:**
```sh
perf stat ./your_program
```
Example command: 
```sh
perf stat sleep 5
```
This is a quick command that shows what kind of information stat returns

Other useful perf commands:
```sh
perf stat -e cache-references,cache-misses,cycles,instructions,branches,branch-misses ./your_program

perf record -e cycles:u ./your_program
perf report --stdio

perf mem record ./your_program
perf mem report --stdio
```
