
#ifndef __PRINTF_H
#define __PRINTF_H
#include <stdarg.h>
void printf(const char *fmt, ...);
int base_print (unsigned long int value,int arg_width,char *finalstr,int iter,int base);
int convert_fullstring(char *fullstr, char *mainstr, va_list args);
#endif
