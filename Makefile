#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#
TARGET=i386-pc-posnk

# define the C compiler to use
CC = @echo " [  CC  ]	" $< ; $(TARGET)-gcc
CX = @echo " [  CXX ]	" $< ; $(TARGET)-gxx
LD = @echo " [  LD  ]	" $@ ; $(TARGET)-gcc

# define the program name and path
PROGNAME=vmpxx
PROGPATH=/boot/vmpxx

# define any compile-time flags
CFLAGS = -Wall -g

# define any directories containing header files other than /usr/include
#
INCLUDES = 

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = 

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = 

# define the C source files
OBJS = $(BUILDDIR)login.o

DIRS = $(BUILDDIR)core $(BUILDDIR)vfs $(BUILDDIR)arch

$(BUILDDIR)arch: 
	@mkdir $(BUILDDIR)arch

$(BUILDDIR)core: 
	@mkdir $(BUILDDIR)arch

$(BUILDDIR)vfs: 
	@mkdir $(BUILDDIR)arch

$(BUILDDIR)vm: 
	@mkdir $(BUILDDIR)arch

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
all:	$(BUILDDIR)$(PROGNAME)

$(BUILDDIR)$(PROGNAME): $(OBJS) $(DIRS)
	$(LD) $(LFLAGS) $(LIBS) -o $(BUILDDIR)$(PROGNAME) $(OBJS)

install: $(BUILDDIR)$(PROGNAME)
	install $(BUILDDIR)$(PROGNAME) $(DESTDIR)$(PROGPATH)

.PHONY: depend clean

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
$(BUILDDIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@
	
$(BUILDDIR)%.oo: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) $(BUILDDIR)*.o $(BUILDDIR)*~ $(BUILDDIR)$(PROGNAME)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

