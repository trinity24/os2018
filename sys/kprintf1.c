#include <stdarg.h>
#include <sys/pmap.h>
#include <stdlib.h>
#include <sys/string.h>
/*int Mystrlen(const char *s)
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
}*/
//Reference used: https://github.com/stevej/osdev/blob/master/kernel/misc/kprintf.c
// decimal value to string
int base_print (unsigned long long value,int arg_width,char *finalstr,int iter,int base)
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
    unsigned long long x=value; int len,remain_width;
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


void clearscreen()
{
	char *video = (char*)0xB8000;
	for(int i=0;i<25;i++)
	{
		for(int j=0;j<80;j++)
		{
			*video++ = ' ';
        		*video++ = 0;	
		}
	}
}
void clearscreen_k()
{
	char *video = (char*)0xB8000+KERNBASE;
        for(int i=0;i<25;i++)
        {
                for(int j=0;j<80;j++)
                {
                        *video++ = ' ';
                        *video++ = 0;
                }
        }

}
//writing to the video output 
//static volatile  char *video = (volatile char*)0xB8000;
 void write_string( int colour, const char *string )
{	
	int l=0;
	//printf("writing to the screen\n");
	  static volatile  char *video = (volatile char*)0xB8000;
    while( *string )
    {
	if(*string=='\n' )
	{
		
		video=video+(80-l)*2;
		string++;
		l=0;
		    
	}
	else {
    	l++;
	if(l > 80)
	   l=0;
        *video++ = *string++;
        *video++ = colour;
	}
    }
}
void scroll_up(int colour)
{
	for(int i=0;i<24;i++)
	{
		//copy_line
		
		char *video_from=(char *)(0xb8000)+((i+1)*160)+KERNBASE;
	        char *video_to = (char *)(0xb8000)+((i)*160)+KERNBASE;
		for(int i=0;i<160;i++)
       		{
                	*video_to++ =*video_from++;
        	}
	}
        return;
}
 void write_string_k( int colour, const char *string )
{
      	static volatile  int vertical=1;
	static volatile int  horizontal=1;
        static volatile  char *video = (volatile char*)0xB8000+KERNBASE;
    	if(video == (char*)0xB8000+KERNBASE)
		clearscreen_k();
	while( *string )
    	{
	
        	if(*string=='\n' )
        	{

                	video=video+(80-vertical+1)*2;
               	 	string++;
                	vertical=1;
			horizontal++;
        	}
        	else 
		{

						
        		//make sure that you clear the line. 
			if(vertical > 80)
			{
				vertical=1;
	   			//clearscreen_k();
				horizontal++;
        		}
			if(horizontal >25)
			{
				scroll_up(colour);
				video=(char*)(0xb8000)+24*160+KERNBASE;
				vertical=1;
				horizontal=25;
			}
			
			*video++ = *string++;
        		*video++ = colour;
			vertical++;
		}
    	}
}


//this function is used to get a full string which is expected to see
//on the screen from the mainstring and args into the "fullstr"
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
			x=base_print((long long int)va_arg(args, long long int),width_arg,fullstr,iter, 16);
			iter=x+iter;
			//break;
        	}
            
        	i++;
       	 }
      }
	fullstr[iter]='\0';
	return 1;

}

int kprintf(char *mainString, ...)
{
    va_list args;
    va_start(args,mainString);
    char finalString[1024];   
    convert_fullstring(finalString,mainString,args);
    va_end(args);
    write_string(11, finalString);
    return 1;
}
 
int kprintf_k(char *mainString, ...)
{
    va_list args;
    va_start(args,mainString);
    char finalString[1024];
    convert_fullstring(finalString,mainString,args);
    va_end(args);
    write_string_k(11, finalString);
    return 1;
}
                                                                                                                                 
