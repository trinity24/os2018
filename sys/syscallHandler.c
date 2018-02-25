#include <sys/syscallHandler.h>
#include <sys/Mysyscalls.h>
#include <sys/kprintf1.h>

uint64_t write_syscall(uint64_t fd, char *buf, uint64_t count)
{
	uint64_t i=0;
	while(count-- && buf[i]!='\0')
	{
		kprintf_k("%c",buf[i]);
		i++;
	}
	return i;
}

void syscallHandler(struct Regs *regs)
{
	kprintf_k("inside syscall handler");
	switch(regs->rax)
	{
		
		case 1: //write syscall
			//writing to the output/console
			if(regs->rbx==1)
				regs->rax=write_syscall(regs->rbx,(char *)regs->rcx,regs->rdx);
			
			break;
	
	
		
	}
	kprintf_k("Coming out of this");
	
	return ;	
}



