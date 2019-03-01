.PHONY: all clean

DEBUG = 0
VER = 1.0.0
KVER = $(shell uname -r)
PWD = $(shell pwd)

EXTRA_CFLAGS := -DDEVICE_VERSION="\"${VER}\""

ifeq ($(DEBUG), 1)
	EXTRA_CFLAGS += -DDEBUG
endif

objs += kpath.o realpath.o

ifeq ($(KVER),$(shell uname -r))
    obj-m += krealpath.o
	krealpath-objs := $(objs)
else
    obj-m += krealpath-$(KVER).o
	krealpath-$(KVER)-objs := $(objs)
endif

all:
	make -C /lib/modules/$(KVER)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVER)/build M=$(PWD) clean
