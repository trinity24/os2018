#include <stdlib.h>
#include <stdio.h>
#include <sys/defs.h>
#include <unistd.h>
#include <sys/printf.h>
#include <sys/Mysyscalls.h>
#include <sys/string.h>
#define KERNBASE 0xffffffff80000000

int  main(int argc, char* argv[], char* envp[])
{
        //puts("Hi! This is SBUSH\n And below will be child 'C' and parent 'P' \n");
        int pid;
	int status;
	pid=fork();
	if(pid==0)
	{
		sleep(10);
		printf("This is child!\n");
		exit(123);
	}
	else
	{
		printf("Hello\n");
		wait(&status);
		printf("This is parent - status %d\n", status);
	}
        return 0;
}
