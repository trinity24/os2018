
#include<stdlib.h>
#include<stdio.h>
#include<sys/printf.h>
int main(int argc, char* argv[], char* envp[])
{

	for (int i = 1;i < argc;i++)
	{
		printf("%s ", argv[i]);
	}
	return 0;
}
