#include <sys/Mysyscalls.h>
#include <sys/defs.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
int putchar(int c)
{
  // write character to stdout
	char ch=c;
		
  	uint64_t len=write(1,&ch,1);
	if(len<=0) return EOF;
	return c;
}
int puts( char *s)
{

        return write(1,s,strlen(s));

}
