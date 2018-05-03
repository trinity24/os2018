
//#include <sys/Mysyscalls.h>
#include <sys/Mysyscalldefs.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
char file_global[300]="";
char buff_global[300];
uint64_t *filedes;
//long MyOpendir()
#define KERNBASE 0xffffffff80000000

pid_t get_pid()
{
	return Myget_pid(39);
}
pid_t get_ppid()
{
        return Myget_ppid(110);
}
uint64_t kill(pid_t pid, int signal)
{
	return MyKill(62,pid,signal);
}
uint64_t wait(int *status)
{
	return Mywait(61,status);
}

uint64_t waitpid(int pid, int *status)
{
	return Mywaitpid(7,pid,status);
}
void ps()
{
	return Myps(10);
}
void exit(int status)
{
	return MyExit(60,status);
}
uint64_t write(uint64_t fd, char * buf, uint64_t count)
{
	return MyWritedef(1,fd,buf,count);

}
uint64_t readdir(int fd, int flags,char *buff)
{
	return Myreaddir(91,fd, flags,buff);
}
uint64_t opendir(char *path, int flags)
{
        return Myopendir(90,path, flags);
}
int close(int fd)
{
	return Myclose(89,fd);
}
uint64_t read(int fd,char *buf,int count)
{
	uint64_t i=0;
	uint64_t ret = MyReaddef(3,fd,buff_global,count);
	
	while(buff_global[i]!='\0')
	{
		buf[i]=buff_global[i];	
		i++;
	}
	buf[i]='\0';	
	return ret;
}


//TODO: gotta change these syscalls later

pid_t fork()
{
	return MyForkdef(57);
}

void execvpe(char *filename, char* args[], char* envp[])
{
	return MyExecdef(59,filename, args, envp);
}
uint64_t pipe(uint64_t *fildes)
{
	filedes= filedes;
	return MyPipedef(42,filedes);
}
int chdir(const char *path)
{
        return Mychdirdef(80,path);

}
unsigned int sleep(unsigned int sec)
{
	Mysleepdef(83,sec);
	return 0;
}
void* malloc(size_t size)
{
	return Mymalloc(81, size);
}
char *getcwd(char *buf, size_t size)
{       
        return Mygetcwd(82,buf,size);
}

int open(const char *pathname, int flags)
{
	char dest[256];
        int src_len = strlen(pathname);
        for(int i=0;i<src_len;i++)
        {
                dest[i]= pathname[i];
        }
        dest[src_len] = '\0';

	return MyOpendef(2,dest,flags);
}
