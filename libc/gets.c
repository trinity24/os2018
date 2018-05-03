

#include <stdio.h>
#include <unistd.h>
char* gets_l(char *readline,int len)
{

        int read_length=0;
        read_length=read(0, readline, len);
        if(read_length)return readline;
        else return NULL;
}
