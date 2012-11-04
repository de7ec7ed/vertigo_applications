TARGET := ios_sysldr

MKDIR := ../../../make
BINDIR := ../../../binaries
INCLUDESDIR := ../../../includes
LIBRARIESDIR := ../../../libraries
SYSTEMDIR := ../../../system

REMOTEUSER:= root
REMOTEIP := 192.168.1.112
REMOTEPATH := /

include $(MKDIR)/Makefile.common

INCFILES := $(INCDIR)/check.h $(INCDIR)/kern.h
INCLUDESFILES := $(INCLUDESDIR)/defines.h $(INCLUDESDIR)/types.h 

HDRFILES := $(INCFILES) $(INCLUDESFILES)

SRCFILES := main.c main.S
SRCFILES += kern.c
SRCFILES += gen.c tt.c

include $(MKDIR)/Makefile_arm_ios.common

CODESIGN_IDENTITY := "0xde7ec7ed"

all:
	$(MAKE) -s clean
	$(MAKE) -s copy
	$(MAKE) -s build
	$(MAKE) -s codesign

copy:
	cp $(LIBRARIESDIR)/armlib/src/vmsa/tt.c $(SRCDIR)/
	cp $(LIBRARIESDIR)/armlib/src/vmsa/gen.c $(SRCDIR)/
	
build:
	mkdir -p $(OBJDIR)
	$(MAKE) -s $(TARGET)

clean:
	rm -rf $(BINDIR)/$(TARGET)
	rm -f $(SRCDIR)/gen.c
	rm -f $(SRCDIR)/tt.c
	rm -rf $(OBJDIR)
	rm -rf *~
	rm -rf $(INCDIR)/*~
	rm -rf $(SRCDIR)/*~


$(OBJDIR)/%.S.o:$(SRCDIR)/%.S $(DEPS)
	echo "$<"
	$(CC) $(CFLAGS) -c -D__ASSEMBLY__ -o $@ $<

$(OBJDIR)/%.c.o:$(SRCDIR)/%.c $(DEPS)
	echo "$<"
	$(CC) $(CFLAGS) -c -D__C__ -o $@ $<

$(TARGET): $(OBJS)
	echo "$(TARGET)"
	$(CC) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(OBJS)

codesign:
	#CODESIGN_ALLOCATE is an environment variable for codesign
	codesign -s $(CODESIGN_IDENTITY) --entitlements entitlements.plist -f $(BINDIR)/$(TARGET)

transfer:
	scp $(BINDIR)/$(TARGET) $(REMOTEUSER)@$(REMOTEIP):$(REMOTEPATH)

connect:
	ssh $(REMOTEUSER)@$(REMOTEIP)

