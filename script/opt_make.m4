define(`ARCH', `define(`ARCHNAME', `$1')dnl')dnl
define(`LINKOPT',`dnl')dnl
define(`DEFTARGET',`dnl')dnl
define(`ALLTARGET',`dnl')dnl
include(`config/opts.list')dnl
define(`ARCH', `dnl')dnl
dnl
dnl
define(`TARGET', `$1_tree:
	@script/cptree src $(BUILDDIR)$1')dnl
include(`opts/targets.list')dnl
dnl
dnl
define(`OPT', `define(`OPTNAME',`$1')include(opts/$1.list)dnl')dnl
define(`OPT_C', `dnl')dnl
define(`OPT_NASM', `dnl')dnl
define(`OPT_GAS', `dnl')dnl
define(`OPT_REG', `dnl')dnl
define(`OPT_INCL', `INCLS_`'OPTNAME`'_$1 += -Iinclude/$2 ')dnl
include(`opts/opts.list')dnl

define(`LINKOPT', `INCLS_$1+=$(INCLS_$2_all)
INCLS_$1+=$(INCLS_$2_`'ARCHNAME)')dnl
include(`config/opts.list')dnl

define(`OPT', `include(opts/$1.list)dnl')dnl
define(`OPT_INCL', `dnl')dnl
define(`OPT_C', `$(BUILDDIR)TARGETNAME/$2.o: src/$2.c $(BUILDDIR)_opt_cfg.h
	$(CC) $(CFLAGS) -DTARGET_`'TARGETNAME $(INCLUDES) $(INCLS_`'TARGETNAME) -c src/$2.c -o $(BUILDDIR)TARGETNAME/$2.o
	$(CDEP) $(CFLAGS) $(INCLUDES) $(INCLS_`'TARGETNAME) -M src/$2.c -o $(BUILDDIR)TARGETNAME/$2.d
')dnl
define(`OPT_NASM', `$(BUILDDIR)TARGETNAME/$2.o: src/$2.s
	$(NASM) $(NASMFLAGS) src/$2.s -o $(BUILDDIR)TARGETNAME/$2.o
')dnl
define(`OPT_GAS', `$(BUILDDIR)TARGETNAME/$2.o: src/$2.S $(BUILDDIR)_opt_cfg.h
	$(GAS) $(GASFLAGS) -DTARGET_`'TARGETNAME $(INCLUDES) $(INCLS_`'TARGETNAME) -c src/$2.S -o $(BUILDDIR)TARGETNAME/$2.o
')dnl
define(`TARGET', `define(`TARGETNAME', `$1')dnl
include(`opts/opts.list')')dnl
include(`opts/targets.list')dnl

define(`OPT', `define(`OPTNAME',`$1')include(opts/$1.list)dnl')dnl
define(`OPT_C', `OBJS_`'TARGETNAME`'_`'OPTNAME`'_$1+=$(BUILDDIR)TARGETNAME/$2.o')dnl
define(`OPT_NASM', `OBJS_`'TARGETNAME`'_`'OPTNAME`'_$1+=$(BUILDDIR)TARGETNAME/$2.o')dnl
define(`OPT_GAS', `OBJS_`'TARGETNAME`'_`'OPTNAME`'_$1+=$(BUILDDIR)TARGETNAME/$2.o')dnl
include(`opts/targets.list')dnl

define(`LINKOPT', `OBJS_$1+=$(OBJS_$1_$2_all)
OBJS_$1+=$(OBJS_$1_$2_`'ARCHNAME)')dnl
include(`config/opts.list')dnl

define(`OPT', `$1_`'TARGETNAME`'_: $(OBJS_`'TARGETNAME`'_$1_all) $(OBJS_`'TARGETNAME`'_$1_`'ARCHNAME)')dnl
include(`opts/targets.list')dnl

define(`TARGET', `$1_`'OPTNAME dnl')dnl
define(`OPT', `$1:define(`OPTNAME', `$1')include(opts/targets.list)')dnl
include(`opts/opts.list')dnl

define(`TARGET', `$1: $(OBJS_$1) $1_tree ld/$1_`'ARCHNAME`'.ld
	$(LD) -T ld/$1_`'ARCHNAME`'.ld $(LFLAGS) $(LIBS) -o $(BUILDDIR)$2 $(OBJS_$1) -lgcc')dnl
include(`opts/targets.list')
dnl
define(`LINKOPT',`dnl')dnl
define(`DEFTARGET',`default: $1')dnl
define(`ALLTARGET',`all: $1')dnl
include(`config/opts.list')dnl

clean:
define(`TARGET', `	$(RM) $(BUILDDIR)$2')dnl
include(`opts/targets.list')dnl
define(`OPT', `include(opts/$1.list)dnl')dnl
define(`OPT_C', `	$(RM) $(BUILDDIR)TARGETNAME/$2.o $(BUILDDIR)TARGETNAME$2.d')dnl
define(`OPT_GAS', `	$(RM) $(BUILDDIR)TARGETNAME/$2.o')dnl
define(`OPT_NASM', `	$(RM) $(BUILDDIR)TARGETNAME/$2.o')dnl
define(`TARGET', `define(`TARGETNAME', `$1')dnl
include(`opts/opts.list')dnl')dnl
include(`opts/targets.list')dnl
