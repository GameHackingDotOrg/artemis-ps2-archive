/*
 * xprinf.h
 *
 * Copyright (C) 2007-2008 misfire <misfire@xploderfreax.de>
 *
 * Licensed under the Academic Free License version 2.0.  See file LICENSE.
 */

#ifndef _XPRINTF_H_
#define _XPRINTF_H_

#include <stdarg.h>

/* Define available functions here */
#define F_vxprintf
#define NOFLOATINGPOINT
#define F___sout

#define F_vsprintf
#define F_sprintf

int vsprintf(char *buf, const char *fmt, va_list ap);
int sprintf (char *str, const char *format, ...);

#endif /*_XPRINTF_H_*/
