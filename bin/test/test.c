#include <stdlib.h>
#include <stdio.h>
#include <sys/defs.h>
#include <sys/Mysyscalls.h>
#define KERNBASE 0xffffffff80000000

int  main(int argc, char* argv[], char* envp[])
{
	char *string="hello world";
	
	//print(string);
	puts(string);
	while(1);
	return 0;
}
