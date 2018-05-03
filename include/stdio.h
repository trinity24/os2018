#ifndef _STDIO_H
#define _STDIO_H
#include <stdarg.h>
static const int EOF = -1;

#define FILE_COUNT 1000

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_DIRECTORY 3
/*
typedef struct FILE
{
 int fd ;
 int flags;
 long offset;
} FILE ;
//initialise files here 
extern FILE all_files[FILE_COUNT];
*/
#define stdin 0
#define stdout 1
#define stderr 2


int putchar(int c);
int puts( char *s);
int puts1( const char *s);
char *gets(char *s);

char* gets_l(char *readline,int len);

int printf(char *mainString, ...);

int base_print (unsigned long int value,int arg_width,char *finalstr,int iter,int base);
int convert_fullstring(char *fullstr, char *mainstr, va_list args);

#endif
