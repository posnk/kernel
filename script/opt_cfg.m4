define(`LINKOPT', ``#'ifdef TARGET_$1
	`#'include "opts/$2.h"
	`#'define OPT_$2
`#'endif')dnl
define(`ARCH', `#define ARCH_$1
#define ARCH_STRING "$1"
#include "arch/$1/config.h"
')dnl
define(`ALLTARGET',`dnl')dnl
define(`DEFTARGET', `dnl')dnl
include(`config/opts.list')dnl
