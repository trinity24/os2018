#include <sys/defs.h>
//#include <string.h>

int Mystrlen(const char *s)
{
        //printf("In strlen fucntion now: \n");
        int size=0;
        if(s==NULL)
                return 0;

        while(*s !='\0')
                {
                        //printf("Now counting started into loop  %c \n",*s);
                        s++;

                        size++;
                }
        return (size);
}

int Mystrcmp(char *s1, char *s2,uint64_t n)
{
    if(s1==NULL)
    {
        if(s2==NULL)
            return 1;
        else
            return 0;
    }
    if(s2==NULL)
        return 0;
    int l1=Mystrlen(s1);
    int l2=Mystrlen(s2);
    
    if(l1!=l2)
    {
        return 1;
    }
    else
    {
        int i=0;
        while(i<n)
        {
            // move in the string from right to left one character each
            if(s1[i]==s2[i])
                i++;
            else
                return 1;
        }
        if(i==n) // if l1=l2=0 then both of them are equal and reached till 
            return 0;
    }
    return 1;
}

int Mystrcmp1(char *s1, char *s2)
{
    if(s1==NULL)
    {
        if(s2==NULL)
            return 1;
        else
            return 0;
    }
    if(s2==NULL)
        return 0;
    int l1=Mystrlen(s1);
    int l2=Mystrlen(s2);

    if(l1!=l2)
    {
        return 1;
    }
    else
    {
        int i=0;
        while(i<l1)
        {
            // move in the string from right to left one character each
            if(s1[i]==s2[i])
                i++;
            else
                return 1;
        }
        if(i==l1) // if l1=l2=0 then both of them are equal and reached till 
            return 0; 
    }
    return 1;
}   
    

