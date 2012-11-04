TARGET := lin_sysldr

# change include/linux/vermagic.h to fix some insmod load issues
##define VERMAGIC_STRING "2.6.35.7-00148-g6d54618 SMP preempt"

MAGIC_PANDABOARD := "\"2.6.35.7-00148-g6d54618 SMP preempt mod_unload ARMv7 \""
MAGIC_NEXUS := "\"2.6.35.7-ge382d80 preempt mod_unload ARMv7 \""
MAGIC_EMULATOR := "\"2.6.29-g46b05b2-dirty mod_unload ARMv5 \""
MAGIC_XT860 := "\"2.6.35.7 SMP preempt mod_unload ARMv7 \""
#MAGIC_SAMSUNG_S3 := "\"3.0.8-704434-user SMP preempt mod_unload modversions ARMv7\""
MAGIC_SAMSUNG_S3 := "\"3.0.42-cyanogenmod-g2a1c8c2 SMP preempt mod_unload modversions ARMv7\""

#just pick something close, you dont have to build it
CONFIG_FILE := msm_defconfig
#CONFIG_FILE := mapphone_defconfig
#CONFIG_FILE := s5pc110_defconfig

PWD := $(shell pwd)
LINKS_DIR := $(PWD)/../../../../
ANDROID_NDKDIR := $(PWD)/../../../../android-ndk
ANDROID_SDKDIR := $(PWD)/../../../../android-sdk


NDKDIR := $(LINKS_DIR)/android-ndk
KDIR := $(LINKS_DIR)/android-kernel
TOOLCHAINDIR := $(ANDROID_NDKDIR)/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
TOOLCHAIN_PREFIX := arm-linux-androideabi-

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
	cp $(LIBRARIESDIR)/armlib/src/vmsa/tt.c src/ttC.c
	cp $(LIBRARIESDIR)/armlib/src/vmsa/tt.S src/ttS.S
	cp $(LIBRARIESDIR)/armlib/src/vmsa/gen.c src/genC.c
	cp $(LIBRARIESDIR)/armlib/src/vmsa/tlb.S src/tlbS.S
	cp $(LIBRARIESDIR)/armlib/src/vmsa/gen.S src/genS.S
	cp $(LIBRARIESDIR)/armlib/src/int.S src/intS.S

build: 
	touch inc/config.h
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TOOLCHAINDIR)/$(TOOLCHAIN_PREFIX) -C $(KDIR) M=$(PWD) modules
	mv $(TARGET).ko $(BINARIESDIR)/

clean:
	rm -f inc/config.h
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TOOLCHAINDIR)/$(TOOLCHAIN_PREFIX) -C $(KDIR) M=$(PWD) clean
	rm -f $(BINARIESDIR)/$(TARGET).ko
	rm -f src/ttC.c
	rm -f src/ttS.S
	rm -f src/genC.c
	rm -f src/tlbS.S
	rm -f src/genS.S
	rm -f src/intS.S
	rm -rf *.ko *.mod.* *.o *.order *.symvers *~ src/*~ inc/*~

kernel_config:
	 $(MAKE) ARCH=arm CROSS_COMPILE=$(TOOLCHAINDIR)/$(TOOLCHAIN_PREFIX) -C $(KDIR) $(CONFIG_FILE)

# frankly you can kill this after a few seconds. it really doesn't matter if it builds correctly. it just needs to produce a few extra config files.
kernel_make:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TOOLCHAINDIR)/$(TOOLCHAIN_PREFIX) -C $(KDIR)

magic_nexus:
	echo "#define VERMAGIC_STRING" $(MAGIC_NEXUS) > $(KDIR)/include/linux/vermagic.h

magic_pandaboard:
	echo "#define VERMAGIC_STRING" $(MAGIC_PANDABOARD) > $(KDIR)/include/linux/vermagic.h
	
magic_xt860:
	echo "#define VERMAGIC_STRING" $(MAGIC_XT860) > $(KDIR)/include/linux/vermagic.h

magic_emulator:
	echo "#define VERMAGIC_STRING" $(MAGIC_EMULATOR) > $(KDIR)/include/linux/vermagic.h

magic_samsung_s3:
	echo "#define VERMAGIC_STRING" $(MAGIC_SAMSUNG_S3) > $(KDIR)/include/linux/vermagic.h
	
transfer:
	#sudo $(ANDROID_SDKDIR)/platform-tools/adb connect 192.168.1.120
	$(ANDROID_SDKDIR)/platform-tools/adb push $(TARGET).ko /system/
	#sudo $(ANDROID_SDKDIR)/platform-tools/adb disconnect
