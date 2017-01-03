/******************************************************************************\
Copyright (C) 2015 Peter Bosch

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
\******************************************************************************/

/**
 * @file include/crt/status.h
 *
 * Part of posnk kernel
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 */

#ifndef __crtstatus__
#define __crtstatus__
#include "config.h"

#define STCODE_OK	(0)
#define STCODE_NOMEM	(1)
#define STCODE_NORES	(2)
#define STCODE_INUSE	(3)
#define STCODE_NOENT	(4)
#define STCODE_NOPERM	(5)
#define STPO		__cst_t __csl; __cst_t *__cst = &__csl
#define STPD		__cst_t *__cst
#define STPC		__cst
#define STPF		( __cst->stcode != STCODE_OK )
/**
 * @brief Function call status structure
 * Used to pass error information to caller
 */
typedef struct {
	int		 stcode;
#ifdef CFG_VERBOSESTATUS
	const char	*stmsg;
	const char	*stfile;
	int		 stln;
#endif
} __cst_t;

#define STRET(Value)	\
	do {\
 		__cst->stcode = STCODE_OK; \
		return (Value); \
	} while(0)

#define STRETV	\
	do {\
		__cst->stcode = STCODE_OK; \
		return; \
	} while(0)

#ifdef CFG_VERBOSESTATUS

#define STERRV(Code, Message)	\
	do {\
		__cst->stcode = (Code); \
		__cst->stmsg = (Message); \
		__cst->stfile = __FILE__; \
		__cst->stln = __LINE__; \
		return; \
	} while(0)
	
#define STERR(Value, Code, Message)	\
	do {\
		__cst->stcode = (Code); \
		__cst->stmsg = (Message); \
		__cst->stfile = __FILE__; \
		__cst->stln = __LINE__; \
		return (Value); \
	} while(0)

#define STERRF(Code, Message, Label)	\
	do {\
		__cst->stcode = (Code); \
		__cst->stmsg = (Message); \
		__cst->stfile = __FILE__; \
		__cst->stln = __LINE__; \
		goto Label; \
	} while(0)

#else

#define STERRV(Code, Message)	\
	do {\
		__cst->stcode = (int) (Message); \
		__cst->stcode = (Code); \
		return; \
	} while(0);

#define STERR(Value, Code, Message)	\
	do {\
		__cst->stcode = (int) (Message); \
		__cst->stcode = (Code); \
		return (Value); \
	} while(0)

#define STERRF(Code, Message, Label)	\
	do {\
		__cst->stcode = (Code); \
		__cst->stmsg = (Message); \
		goto Label; \
	} while(0)

#endif


#endif
