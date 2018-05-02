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
		//sleep(10);
		
		printf("This is child!\n");
		char buf[10];
		getcwd(buf,10);
		printf("Current working directory : %s\n",buf);
		
		chdir("bin/test/../test/..");
		printf("Changing working directory to bin/test/../test\n");
		getcwd(buf,10);
		printf("Current working directory : %s\n",buf);
		
		int fd=open("docs/sharmila.txt",2);
		write(fd,"saichandisawesome",10);
		close(fd);
		fd=open("docs/sharmila.txt",2);
		read(fd,buf,10);
		printf("%s is read\n",buf);
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
