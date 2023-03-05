sudo mknod -m 777 /dev/chardev c 240 0

strace -e lseek ./selftest1 /dev/chardev
make -C /lib/modules/$(uname -r)/build M=$PWD
./pa2grader /dev/chardev
