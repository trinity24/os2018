
#include <sys/Mysyscalls.h>
#include <sys/Mysyscalldefs.h>
char file_global[300]="";
char buff_global[300];
uint64_t *filedes;
//long MyOpendir()
#define KERNBASE 0xffffffff80000000

void print(char *s)
{
	
	char *video = (char*)(0xB8000+KERNBASE);
        
        while(s!='\0')
        {
                *video=*s;
                video++;
                s++;
        }
	return;
}
uint64_t MyOpen(const char *file,uint64_t flag, uint64_t mode)
{
	int i=0;
	while(file[i]!='\0')
	{
		file_global[i]=file[i];
	}
	


	return MyOpendef(2,file_global,flag,777);


}

uint64_t MyWrite(uint64_t fd, char * buf, uint64_t count)
{
	int i=0;
	while(buf[i]!='\0')
	{
		buff_global[i]=buf[i];
		i++;
	}
	return MyWritedef(1,fd,buff_global,count);

}
uint64_t MyRead(uint64_t fd,char *buf,uint64_t count)
{
	uint64_t i=0;
	uint64_t ret = MyReaddef(0,fd,buff_global,count);
	
	while(buff_global[i]!='\0')
	{
		buf[i]=buff_global[i];	
		i++;
	}
	buf[i]='\0';	
	return ret;
}


//TODO: gotta change these syscalls later

pid_t MyWaitpid(pid_t pid,uint64_t *stat_add, uint64_t options)
{
/*	int i=0;
	while(file[i]!='\0')
	{
		buff_global[i]=file[i];
	}

	return MyWaitpiddef(7,pid, buff_global,options)
*/	return -1;
}
pid_t MyFork()
{
	return MyForkdef(2);
}
uint64_t MyPipe(uint64_t *fildes)
{
	filedes= filedes;
	return MyPipedef(42,filedes);
}


