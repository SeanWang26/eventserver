#ifndef JTPRINTF_H
#define JTPRINTF_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

int jtprintf(const char* fmt, ...);
int jtprintfenable(int enable);
int jtprintflip();

#ifdef __cplusplus
}
#endif

#endif
