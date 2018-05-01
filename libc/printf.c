
#include <stdarg.h>
#include <stdlib.h>
#include <sys/string.h>
#include <stdio.h>
int base_print (unsigned long int value,int arg_width,char *finalstr,int iter,int base)
{

        if(value==0)
        {
                finalstr[iter++]='0';
                return 1;
        }
    int initial= iter;
    char s[10]; int i=0;
    int y=value%base;
   // int k; 
    unsigned int x=value; int len,remain_width;
    while(x!=0)
    {       
            if(y>=10)
                    s[i++]='A'+y-10;
            else    
                    s[i++]=y+'0';
            x=x/base;
            y=x%base;
     
    }
    len=strlen(s);

    //l=Mystrlen(finalstr);
        remain_width= arg_width- len;
    while(remain_width>0)
    {   
        finalstr[iter++]=0+'0';
        
        remain_width--;
        }
   len=strlen(s);
    int j=0;
    for(j=0;j<len;j++)
        finalstr[iter++]=s[len-j-1];
    //finalstr[iter]='\0';
    return iter-initial;

}
int convert_fullstring(char *fullstr, char *mainstr, va_list args)
{
    int i=0,iter=0; int x;
    int len=strlen(mainstr);
    char *s;
    while(i<len)
    {
        if(mainstr[i]!='%')
            fullstr[iter++]=mainstr[i++];
        else
        {
            int width_arg=0;
            i++;
            while(mainstr[i]>'0' && mainstr[i]<'9') //if we have an integer
            {
                width_arg*=10;
                width_arg+=mainstr[i]-'0';
                i++;
            }
            switch(mainstr[i])
            {
                case 'd':
                         x=base_print((int)va_arg(args, int), width_arg, fullstr, iter,10);
                         iter=x+iter;
                         break;
                case 'c':
                         fullstr[iter++]= (char)(va_arg(args,int));
                         break;
                case 's':
                         s= (char *)(va_arg(args,char *));
                         while(*s)
                         {
                             fullstr[iter++]=*s;
                             s++;
                         }
                        break;
                case 'x':
                        fullstr[iter++]='0';
                        fullstr[iter++]='x';
                        x=base_print((unsigned long int)va_arg(args, unsigned long int), width_arg, fullstr, iter,16);
                        iter=x+iter;
                        break;
                case 'p':
                        x=base_print((long int)va_arg(args, long int),width_arg,fullstr,iter, 16);
                        iter=x+iter;
                        //break;
                }

                i++;
                                      
         }
      }
      fullstr[iter]='\0';
      return 1;

}

int printf(char *mainString, ...)
{
    va_list args;
    va_start(args,mainString);
    char finalString[1024];
    convert_fullstring(finalString,mainString,args);
    va_end(args);
    puts(finalString);
    return 1;
}

