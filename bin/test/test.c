

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/defs.h>
#include <sys/Mysyscalls.h>
#include <sys/string.h>
#define KERNBASE 0xffffffff80000000

void exec_shell(char *command, int background)
{
	char content[10][20];
        char *args[10];
        int status,pid;
        strtok(command," ", content[0]);
        args[0] = content[0];

	for (int i = 1;i < 10;i++)
	{
		args[i] = strtok(NULL, " ", content[i]);
		if (!strcmp1(args[i], "&"))
			args[i] = NULL;

		if (args[i] == NULL)
			break;
	}
        
	pid=fork();
        if(pid==0)
        {
        	execvpe(args[0], args, NULL);
        }
        else
	{
		if (!background)
		{
                	waitpid(pid,&status);
		}
	}
        return;
}
void cd_shell(char *command)
{
        //char *tokensString_command;
        //tokensString_command=
	char tokensString_path[100];
        strtok(command," ", tokensString_path); // Ignore cd
        strtok(NULL," ", tokensString_path);

        if(chdir(tokensString_path)==-1){
                printf("error in chdir\n");
        }
        char str3[1024];
     	if(getcwd(str3, sizeof(str3))==NULL){
        	//printf("error in getcwd\n");
    	}

    	printf("%s\n", str3);
    return;
}

void execute_command(char *readline)
{
    	char command[100];
    	strtok(readline," ", command);
    	if(first_occurence(readline,'&')!=-1)
        	exec_shell(readline, 1);
    	else if(!strcmp1(command,"cd"))
        	cd_shell(readline);
    	else
        	exec_shell(readline, 0);
	
 /*   	else if(!strcmp1(command,"sh"))
    	{
        	command=strtok(NULL," ");
        	execute_script(command);
    	}
    	else if(strcmp1(command,"export")==0 || strcmp1(command,"PS1")==0)
        	set_variable(readline);	
	else printf("Sorry the command you enetered is invalid. Please find one of the below : cd /path, ls /path, ls &, sh x.sh, set_variables(path and ps1)\n");
*/
    return;
}
void initialise_shell()
{
	char readline[100];
        do
        {
                puts("sbush> ");
                gets_l(readline,100);
                execute_command(readline);
        }while(strcmp1(readline,"exit")!=0);
	
        return ;
}
int  main(int argc, char* argv[], char* envp[])
{
	initialise_shell();

	return 0;	
	
}
/*{
	int count=0;
	char *buffer=NULL;
	printf("Inside test\n");
	//while(1);
	printf("SBU UNIX\n");
	count= read(1,buffer);
	printf("something is here\n");
	if(count)
		printf("%s is the read string\n", buffer);

	uint64_t pid1,pid2;
	pid1=fork();
	if(pid1==0)
	{
		printf("This is child1\n");
		ps();
		pid2=fork();
		if(pid2==0)
		{
			
			printf("This is child2\n");
			printf("Listing Processes .... \n");
			ps();
			
		}
		else
		{
	//		waitpid(2);	
			printf("After the child 1 has waiting for its children\n");
			ps();	
			printf("Child1 after fork called inside it \n");
		
		}
		
	}
	else
	{
		
		printf("Parent \n");
		
	}
	printf("Out of all processes :Only Parent process is Alive now\n Confirming ...\n");
	ps();
	while(1);
	return 0;
}



*/
