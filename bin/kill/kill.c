
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
int atoi(char *s)
{
        int value=0;
        for(int i=0;i<strlen(s);i++)
                value =(value*10 + s[i] )-'0';
        return value;
}
int main(int argc, char* argv[], char* envp[])
{
	int fail=0;
	//kill -9 pid is the format 
	if(argc <3)
	{
		fail=1;	
	}
	if(!atoi(argv[2])  || strcmp1(argv[1],"-9"))
	{
		fail=1;	
	}
	else
	{	
		if(!fail) 
		{
			kill(atoi(argv[2]));	
		}
		else
		{
			 printf("Error: kill -9 PID is the format to kill\n");
			exit(-1);
		}
	}	
	return 0;
}	


