#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "jtprintf.h"

static int jtshowlog = 1;

int jtprintf(const char* fmt, ...)
{
	int cnt = 0;
	if(jtshowlog)
	{
#if (defined(WIN32) || defined(WIN64))
		//OutputDebugString("");
#else
		va_list argptr;
		va_start(argptr, fmt);
		cnt = vfprintf(stderr, fmt, argptr);
		//cnt = vfprintf(stdout, fmt, argptr);
		va_end(argptr);
#endif
	}
	
	return cnt ;
}

int jtprintfenable(int enable)
{
	jtshowlog = enable;
	return jtshowlog;
}
int jtprintflip()
{
	jtshowlog = ~jtshowlog;
	return jtshowlog;
}

