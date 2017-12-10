#include <sys/defs.h>
int  main()
{
	char *string="hello world";
	char *video = (char*)0xB8000;
        while(string!=NULL)
        {
         	*video++ =*string++;
        }
	while(1);
	return 0;
}
