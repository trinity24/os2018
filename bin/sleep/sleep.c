#include<stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int atoi(char *s)
{
	int value=0;
	for(int i=0;i<strlen(s);i++)
		value =(value*10 + s[i] )-'0';
	return value;
}
int main(int argc, char* argv[], char* envp[])
{
	
	// Error if no args are given to sleep
	if(argc<=1)	
	{
		printf("Error: No arguments given to sleep");
	}
	int sec= atoi(argv[1]);	
	if(sec>0)
	{		
		sleep(sec);
		exit(-1);
	}
	else printf("Error as sleep take only values >0\n");
	return 0;
}
