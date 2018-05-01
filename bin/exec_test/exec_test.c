#include <stdlib.h>
#include <stdio.h>
#include <sys/defs.h>
#include <sys/Mysyscalls.h>
#define KERNBASE 0xffffffff80000000

int  main(int argc, char* argv[], char* envp[])
{
        puts1("In exec_test: Sharmila\n");
        //puts("Hi! This is SBUSH\n And below will be child 'C' and parent 'P' \n");
        while(1);
        return 0;
}
