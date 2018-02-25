

#include <sys/Mysyscalldefs.h>
int ret;

uint64_t MyOpendef(uint64_t syscallNum,const char *file,uint64_t flag, uint64_t  mode)
{
	__asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(file), "c"(flag),"d"(mode):"memory");
	return ret;
}
uint64_t Mygetdentsdef(uint64_t syscallNum,uint64_t fd, void * mydirent, uint64_t count)
{
	__asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(fd), "c"(mydirent),"d"(count):"memory");
	return ret;
}


uint64_t MyWritedef(uint64_t syscallNum,uint64_t fd, char * buf, uint64_t count)
{
	__asm__ volatile(
			  "int $0x80;"
			  "movq %%rax,%1;"
			   :"=a"(ret): "a"(syscallNum),"b"(fd), "c"(buf),"d"(count):"memory");

	return ret;
}

 uint64_t MyReaddef(uint64_t syscallNum,uint64_t fd, char *buf,uint64_t count)
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

 uint64_t MyPipedef(uint64_t syscallNum,uint64_t *filedes)
{
	__asm__ __volatile__ ("int $0x80":"=a"(ret):"a"(syscallNum), "b"(filedes):"memory");
	return ret;
}
