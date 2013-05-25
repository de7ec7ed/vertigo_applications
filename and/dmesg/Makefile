TARGET := and_dmesg

MKDIR := ../../../make
BINDIR := ../../../binaries
INCLUDESDIR := ../../../includes
LIBRARIESDIR := ../../../libraries

include $(MKDIR)/Makefile.common

INCFILES := $(INCDIR)/check.h
INCLUDESFILES := $(INCLUDESDIR)/defines.h $(INCLUDESDIR)/types.h 

HDRFILES := $(INCFILES) $(INCLUDESFILES)

SRCFILES := main.c main.S

include $(MKDIR)/Makefile_arm_and.common

all:
	$(MAKE) -s clean
	$(MAKE) -s build
	
build:
	mkdir -p $(OBJDIR)
	$(MAKE) -s $(TARGET)

clean:
	rm -rf $(BINDIR)/$(TARGET)
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
