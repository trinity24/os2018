#include <stdlib.h>
#include <stdio.h>
#include <sys/defs.h>
#include <sys/Mysyscalls.h>
#define KERNBASE 0xffffffff80000000

int  main(int argc, char* argv[], char* envp[])
{
	char *string="hello world";
	//for(int i=0;i<30;i++)
	uint64_t pid= MyFork();
	if(pid==0)
	{
		puts("I am child");
	}
	else
	{
		puts(string);
	}
	//puts(string);
	//puts("successful \n");
	//puts("sharmila common\n");
	while(1);
	return 0;
}
