#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/printf.h>
int main(int argc, char* argv[], char* envp[])
{
	
	if (fork() == 0)
	{
		while(1)
		{
			for (int i = 0;i < 100000;i++)
				for (int j = 0;j < 2000;j++);
			printf("CHILD**************\n");
		}
	}
	else
	{
		while(1)
		{
			for (int i = 0;i < 100000;i++)
				for (int j = 0;j < 2000;j++);
			printf("PARENT\n");
		}
	}
	return 0;
}
