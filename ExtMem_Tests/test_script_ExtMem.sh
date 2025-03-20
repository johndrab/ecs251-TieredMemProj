export DRAMSIZE=134217728000
export SWAPDIR=/dev/sda99  
#export SWAPDIR=/
export LD_PRELOAD=/users/jpdrab/ExtMem/src/libextmem-default.so  #Replace with your file path

#Put command to run the desired program below:

#./memory_test
stress --vm 4 --vm-bytes 25G --timeout 30s
#./file_processer file1.dat file2.dat file3.dat
#./bench_markDC

