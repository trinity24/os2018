#include <stdio.h>
int puts1(const char *s)
{
  for( ; *s; ++s) if (putchar(*s) != *s) return EOF;
  return (putchar('\n') == '\n') ? 0 : EOF;
}
/*int puts1(const char *s)
{
	
	return MyWrite(1,s,Mystrlen(s));
	
}*/
