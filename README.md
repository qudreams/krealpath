## krealpath

### Note
> krealpath is a simple kernel module to get real-path in linux kernel

### Usage
1. compile
> make
2. add kernel-module krealpath.ko:
> /sbin/insmod krealpath.ko spath=./
3. show result
> dmesg
4. remove kernel-module
> /sbin/rmmod krealpath.ko

