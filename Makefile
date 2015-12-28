#
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#
TARGET=i386-elf

# define the C compiler to use

M4   = @echo " M4	" $< ; m4
GAS  = @echo " GAS	" $< ; $(TARGET)-gcc
NASM = @echo " NASM	" $< ; nasm
CC   = @echo " CC	" $< ; $(TARGET)-gcc
CDEP = @$(TARGET)-gcc
CX   = @echo " CXX	" $< ; $(TARGET)-gxx
LD   = @echo " LD	" $@ ; $(TARGET)-gcc
RM   = @rm -f

# define the program name and path
PROGNAME=vmpxx
PROGPATH=/boot/vmpxx

# define any compile-time flags
CFLAGS=-Wall -g -ffreestanding

NASMFLAGS = -felf -g

# define any link-time flags
LFLAGS = -ffreestanding -O2 -nostdlib -static-libgcc -g

INCLUDES= -iquoteinclude -iquote$(BUILDDIR) -iquote.

config: $(BUILDDIR)opt_make $(BUILDDIR)_opt_cfg.h

$(BUILDDIR)opt_make: script/opt_make.m4 config/opts.list
	$(M4) -I . script/opt_make.m4 > $(BUILDDIR)opt_make

$(BUILDDIR)_opt_cfg.h: script/opt_cfg.m4 config/opts.list
	$(M4) -I . script/opt_cfg.m4 > ${BUILDDIR}_opt_cfg.h

-include $(BUILDDIR)opt_make

clean: cleanconfig

cleanconfig:
	$(RM) $(BUILDDIR)opt_make
	$(RM) $(BUILDDIR)_opt_cfg.h

.PHONY: config clean
