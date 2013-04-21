TARGET := chr_krnldr

# see http://docs.kali.org/development/custom-chromebook-kernel-image for details on building the kernl

PWD := $(shell pwd)

KDIR := ../../../../chromium-kernel
TOOLCHAIN_PREFIX := arm-linux-gnu-

MODULEDIR := $(PWD)

INCLUDESDIR := ../../../includes

LIBRARIESDIR := ../../../libraries

BINARIESDIR := ../../../binaries

EXTRA_CFLAGS := -march=armv7-a -fno-pic -nostdinc -I $(src)/$(INCLUDESDIR) -I $(src)/inc -D__C__

EXTRA_AFLAGS := -march=armv7-a -fno-pic -nostdinc -I $(src)/$(INCLUDESDIR) -I $(src)/inc -D__ASSEMBLY__

obj-m += $(TARGET).o

$(TARGET)-objs := src/file.o src/main.o src/binary.o src/sync.o src/ttC.o src/ttS.o src/genC.o src/genS.o src/tlbS.o src/intS.o

all:
	$(MAKE) -s clean
	$(MAKE) -s copy
	$(MAKE) -s build

copy:
	cp $(LIBRARIESDIR)/armv7lib/src/vmsa/tt.c src/ttC.c
	cp $(LIBRARIESDIR)/armv7lib/src/vmsa/tt.S src/ttS.S
	cp $(LIBRARIESDIR)/armv7lib/src/vmsa/gen.c src/genC.c
	cp $(LIBRARIESDIR)/armv7lib/src/vmsa/tlb.S src/tlbS.S
	cp $(LIBRARIESDIR)/armv7lib/src/vmsa/gen.S src/genS.S
	cp $(LIBRARIESDIR)/armv7lib/src/int.S src/intS.S

build: 
	touch inc/config.h
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TOOLCHAIN_PREFIX) -C $(KDIR) M=$(PWD) modules
	mv $(TARGET).ko $(BINARIESDIR)/

clean:
	rm -f inc/config.h
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TOOLCHAIN_PREFIX) -C $(KDIR) M=$(PWD) clean
	rm -f $(BINARIESDIR)/$(TARGET).ko
	rm -f src/ttC.c
	rm -f src/ttS.S
	rm -f src/genC.c
	rm -f src/tlbS.S
	rm -f src/genS.S
	rm -f src/intS.S
	rm -rf *.ko *.mod.* *.o *.order *.symvers *~ src/*~ inc/*~

# frankly you can kill this after a few seconds. it really doesn't matter if it builds correctly. it just needs to produce a few extra config files.
kernel_make:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TOOLCHAIN_PREFIX) -C $(KDIR)
	
transfer:

