#include <sys/defs.h>
#include <string.h>

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

int MyfindElement(char *s,char c)
{
    int i=0;
    while(s[i]!='\0')
    {
        if(s[i]==c)
            return i;
       	i++;

    }
    return -1;
}
/*
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
*/
int Mystrncmp(char *s1,char *s2,int n)
{
	int i=0;
	int l1=Mystrlen(s1);
	int l2=Mystrlen(s2);
	if(n>l1 || n>l2)
		return 0;

	for(i=0;i<n;i++)
	{
		if(*(s1+i)!=*(s2+i))
			return -1;
	}
	return 0;
}
/*
char * Mystrtok(char *str,char *delim)
{
	static char *prev;int i=0;


	int index=0,min_index;char *token=NULL;
	//if str =NULL then you say the str is the prev string
    if (!str) {
    	str=prev;
    	if(str==NULL)
		return NULL;
    }
    if (!str) return NULL;
    if (str) {
    	min_index=Mystrlen(str);
    	while(*delim!='\0')
    	{	
    		index = MyfindElement(str, *delim);
    		if(index!=-1)
    		{
    			if(index<min_index)
    			{
    				min_index=index;
    					
    			}

    			

    		}
    		delim++;
    	}
    	if(Mystrlen(str)==min_index)
   			prev=NULL;
   		else
   			prev=str+min_index+1;
    	if(index)
    	{    		
    		//copy the token to a new token	
    		token=(char *)malloc(sizeof(char *)*(min_index));
    		for(i=0;i<min_index;i++)
    		{

    			token[i]=str[i];
    		}
    	}
    		//change the give the string value to the prev. 
    		
    }	
    return token;

}

char * Mystrcat(char *s1, char *s2)
{
	int l1=Mystrlen(s1);
	int l2=Mystrlen(s2);
	char *s=(char *)(malloc(sizeof(char *)*(l1+l2)));
	if(s1==NULL)
		return s2;
	if(s2==NULL)
		return s1;
	if(s1==NULL && s2==NULL)
		return NULL;
	int i=0;
	int k1=l1,k2=l2;
	while(k1)
	{
		s[i]=s1[i];
		k1-=1;
		i++;
	}
	i=0;
	while(k2)
	{
		s[l1+i]=s2[i];
		i++;
		k2-=1;
	}

	s[l1+l2]='\0';
	return s;
}

int Mystrcmp(char *s1, char *s2)
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
 
void getcwd(char *cwd)
{
	
	for(int i=0;i<strlen(curr_task->cwd);i++)
	{
		*cwd+i= *((curr_task->cwd)+i);
		
	}
}
*/
