

#include <sys/Mysyscalldefs.h>
uint64_t ret;




uint64_t Myclose(uint64_t syscallNum,int fd )
{
        __asm__ volatile(
                          "int $0x80;"
                          "movq %%rax,%1;"
                           :"=a"(ret): "a"(syscallNum),"b"(fd):"memory");

        return ret;
}


uint64_t MyOpendef(uint64_t syscallNum, char *file,int flags)
{
	__asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(file), "c"(flags):"memory");
	return ret;
}
uint64_t Myopendir(uint64_t syscallNum, char *file,int flags)
{
        __asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(file), "c"(flags):"memory"); 
	return ret;
}

uint64_t Myreaddir(uint64_t syscallNum, int fd,int flags,char *buff)
{
        __asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(fd), "c"(flags),"d"(buff):"memory");
         return ret;
}

uint64_t Mygetdentsdef(uint64_t syscallNum,uint64_t fd, void * mydirent, uint64_t count)
{
	__asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(fd), "c"(mydirent),"d"(count):"memory");
	return ret;
}
uint64_t MyWritedef(uint64_t syscallNum,int fd, char * buf, uint64_t count)
{
        __asm__ volatile(
                          "int $0x80;"
                          "movq %%rax,%1;"
                           :"=a"(ret): "a"(syscallNum),"b"(fd), "c"(buf),"d"(count):"memory");

        return ret;
}
uint64_t Myget_pid(uint64_t syscallNum)
{       
        __asm__ volatile( 
                          "int $0x80;"
                          "movq %%rax,%1;"
                           :"=a"(ret): "a"(syscallNum):"memory");
        
        return ret;
}

void Myps(int syscallNum)
{
         __asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum):"memory");

}
uint64_t MyKill(int syscallNum,pid_t pid, int signal)
{
	 __asm__ volatile(
                          "int $0x80;"
                           :: "a"(syscallNum),"b"(pid), "c"(signal):"memory");
	__asm__ volatile("movq %%rax,%0":"=b"(ret):);
	return ret;
}

uint64_t Mywait(int syscallNum,int *status)
{
	 __asm__ volatile(
                          "int $0x80;"
                           :: "a"(syscallNum),"b"(status):"memory");
         __asm__ volatile("movq %%rax,%0":"=b"(ret):);
	return ret;
}
uint64_t Mywaitpid(int syscallNum, int pid, int *status)
{
	 __asm__ volatile(
                          "int $0x80;"
                           :: "a"(syscallNum),"b"(pid),"c"(status):"memory");
         __asm__ volatile("movq %%rax,%0":"=b"(ret):);
        return ret;

}
/*
pid_t Myget_pid(int syscallNum)
{
         __asm__ volatile(
                          "int $0x80;"
                           :: "a"(syscallNum):"memory");
	
         __asm__ volatile("movq %%rax,%0":"=b"(ret):);
        return ret;
}*/
pid_t Myget_ppid(int syscallNum)
{
         __asm__ volatile(
                          "int $0x80;"
                           :: "a"(syscallNum):"memory");
         __asm__ volatile("movq %%rax,%0":"=b"(ret):);
        return ret;
}


void MyExit(int syscallNum,int status)
{
	 __asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(status):"memory");

}
int Mychdirdef(int syscallNum,const char *path)
{
	         __asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(path):"memory");
	return ret;
}
 uint64_t MyReaddef(uint64_t syscallNum,int fd, char *buf,int count)
{
	__asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(fd), "c"(buf),"d"(count):"memory");	
	return ret;
}

 pid_t MyWaitpiddef(uint64_t syscallNum,pid_t pid,uint64_t *status, uint64_t options)
{
	__asm__ __volatile__ ("int $0x80":"=a"(ret): "a"(syscallNum),"b"(pid), "c"(status), "d"(options):"memory");
	return ret;
}

 pid_t MyForkdef(uint64_t syscallNum)
{
	__asm__ __volatile__ ("int $0x80":"=a"(ret): "a"(syscallNum):"memory");
	return ret;
}
 void MyExecdef(uint64_t syscallNum,char *filename, char* args[], char* envp[])
{
        __asm__ __volatile__ ("int $0x80":"=a"(ret): "a"(syscallNum), "b"(filename), "c"(args), "d"(envp):"memory");
        return ;
}

 uint64_t MyPipedef(uint64_t syscallNum,uint64_t *filedes)
{
	__asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(filedes):"memory");
	return ret;
}
 
void* Mymalloc(uint64_t syscallNum, int size)
{
	__asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"((uint64_t)size):"memory");
	return (void*)ret;
}
void* Mygetcwd(uint64_t syscallNum, char *buf, int size)
{


	 __asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(buf),"c"(size):"memory");
	return (void*)0;

//	__asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum),"b"(buf) ,"c"((uint64_t)size):"memory");
  //      return (void*)ret;
}
void Mysleepdef(uint64_t syscallNum, int size)
{
	  __asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"((uint64_t)size):"memory");
	
}
