#include <sys/syscallHandler.h>
#include <sys/Mysyscalls.h>
#include <sys/kprintf1.h>
#include <sys/task.h>
#include <sys/process.h>
#include <sys/string.h>

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_FORK 57
#define SYS_EXEC 59
#define SYS_PS 
#define SYS_KILL 62
#define SYS_EXIT 60
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_WAIT 61
#define SYS_WAITPID 
#define SYS_GETPID 39
#define SYS_GETPPID 110
#define SYS_LSEEK
#define SYS_CHDIR
#define SYS_GETCWD


uint64_t syscallHandler(struct Regs *regs)
{


	switch(regs->rax)
	{
	
		case 1: //write syscall
			//writing to the output/console
			regs->rax=write_syscall((int)regs->rbx,(char *)regs->rcx,(int)regs->rdx);
			return regs->rax;	
		case 2: 
			regs->rax= open_syscall((char *)regs->rbx, regs->rcx);
			return regs->rax;
		
		case 3:
			regs->rax=read_syscall((int)regs->rbx,(char *)regs->rcx,(int)regs->rdx);
			return regs->rax;
		case 7: 
			regs->rax= waitpid_syscall(regs->rbx,(int *)regs->rcx);
			return regs->rax;
		case 10:
			ps_syscall();
			break;
		case 11: 
			//exec((char *)regs->rbx);
			break;	
		case 39:
			regs->rax=(uint64_t)get_pid();
			return regs->rax;
		case 57:
			regs->rax=fork_syscall();
                        if (curr_task->kstack[511] == 92736)
                        {
                                curr_task->kstack[511] = 0;
                                return 0;
                        }
                        return regs->rax;
		case 59:
                        exec((char *)regs->rbx, (char **)regs->rcx,(char **)regs->rdx);
                        break;
		case 60: 
			exit_syscall(regs->rbx);
			break;
		case 61: 
			regs->rax= wait_syscall((int *)regs->rbx);
			return regs->rax;
		case 80:
			regs->rax = chdir_syscall((char *)regs->rbx);
			return regs->rax;
		case 82:
			regs->rax= (uint64_t)getcwd_syscall((char *)regs->rbx,(int)regs->rcx);
			return regs->rax;
		case 83:
			sleep_syscall((int)regs->rbx);
			return regs->rax;
		case 89:
			kprintf_k("%d is fd in syscallhandler\n",regs->rbx);
			regs->rax= close_syscall(regs->rbx);
			return regs->rax;
		case 110:
			regs->rax=(uint64_t)get_ppid();
                        return regs->rax;
		}
	
	return 0;	
}



