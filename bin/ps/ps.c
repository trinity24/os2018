#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[], char* envp[])
{
	//ps should have only one argument
	if(argc==1)	
	{
		ps();
		return 0;
	}
	else
	{
		printf("Error: ps command takes no arguments\n");
		exit(-1);
	}
}
