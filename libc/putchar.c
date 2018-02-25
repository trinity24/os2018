#include <sys/Mysyscalls.h>
#include <sys/defs.h>
#include <stdio.h>
int putchar(int c)
{
  // write character to stdout
	char ch=c;
		
  	uint64_t len=MyWrite(1,&ch,1);
	if(len<=0) return EOF;
	return c;
}
