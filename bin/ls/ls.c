
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void clear_buf(char *buf,int len)
{
	for(int i=0;i<len;i++)
		buf[i]=0;
}
int main(int argc, char* argv[], char* envp[])
{
	char file[256];
	//ls can have more than 1 arg
	if(argc==1)
	{	
		//list the contents in current working directory
		clear_buf(file,256);
		getcwd(file,1);
		int fd=opendir(file, 1);
		if(fd<0)
		{
			printf("Error: in opening the file\n");	
			exit(-1);
		}
		char buff[256];
		char prev[256];
		clear_buf(buff,256);
		clear_buf(prev,256);
		while(readdir(fd,1,buff)==1)
		{
			if(strcmp1(prev,buff))
			{
				printf("%s\n",buff);
			}
			clear_buf(prev,256);
			strcpy(prev,buff);
			clear_buf(buff,256);
		}
		
	}	
	if(argc==2)
	{
		strcpy(file,argv[1]);
		int fd=opendir(file,1);
                if(fd<0)
                {
                        printf("Error: in opening the file\n"); 
                        exit(-1);
                }
		char buff[256];
                char prev[256];
                for(int i=0;i<256;i++)file[i]=0;
                for(int i=0;i<256;i++)file[i]=0;
                while(readdir(fd,1,buff)==1)
                {
                        if(strcmp1(prev,buff))
                        {
                                printf("%s\n",buff);
                        }
			clear_buf(prev,256);
                        strcpy(prev,buff);
                	clear_buf(buff,256);
		}
		return 0;
	}



	printf("Error: ls can only take two args : 'ls filename'\n");
	exit(-1);
}
