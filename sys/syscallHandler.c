#include <sys/syscallHandler.h>
#include <sys/Mysyscalls.h>
#include <sys/kprintf1.h>
#include <sys/task.h>
uint64_t write_syscall(uint64_t fd, char *buf, uint64_t count)
{
	uint64_t i=0;
	while(count-- && buf[i]!='\0')
	{
		kprintf_k("%c",buf[i]);
		i++;
	}

	schedule();
	return i;
}

uint64_t syscallHandler(struct Regs *regs)
{
	switch(regs->rax)
	{
		
		case 1: //write syscall
			//writing to the output/console
			if(regs->rbx==1)
				regs->rax=write_syscall(regs->rbx,(char *)regs->rcx,regs->rdx);
			
			break;
		case 2:
			regs->rax=fork();	
			if (curr_task->kstack[511] == 92736)
			{
				curr_task->kstack[511] = 0;
				return 0;
			}
			return regs->rax;	
		
	}
	
	return 0;	
}



