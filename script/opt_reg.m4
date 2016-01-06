define(`OPT', `include(opts/$1.list)dnl')dnl
define(`OPT_OBJ', ``	${BUILDDIR}$1 \'')dnl
define(`OPT_REG', ``''

OBJS = ${BUILDDIR}gen/optmgr.go \
include(`config/opts.list')dnl

clean:
define(`OPT_OBJ', ``	$(RM) ${BUILDDIR}$1'')dnl
	$(RM) ${BUILDDIR}gen/optmgr.go
include(`config/opts.list')dnl
