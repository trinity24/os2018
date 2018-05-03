
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char* argv[], char* envp[])
{
       
	 //cat should have two arguments
	if(argc<2)
		printf("Error : Cat takes a filename");
	int fd=open(argv[1],O_RDONLY);
	if(fd <0)
	{
		printf("Error: Invalid file");	
	}
	char buff[256];
	int count = read(fd,buff,255);
	if(count) printf("%s\n",buff);	
       	close(fd);
	return 0;
}
 
