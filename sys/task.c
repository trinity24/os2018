#include <sys/defs.h>
#include <sys/kprintf1.h>
#include <sys/task.h>
#include <sys/pmap.h>

pcb *t1,*t2,*t3;



void schedule(pcb *current_task,pcb *next_task)
{
	context_switch_routine(current_task, next_task);
}

void function1()
{
	kprintf_k("HEY I AM TASK1 -- In function task-1 now \n");
	schedule(t1,t2);
	kprintf("Done with it\n");
	return;
}
void function2()
{
	
	kprintf_k("HEY I AM TASK2 -- In function task-2 now \n");
	schedule(t2,t3);
	kprintf_k("HEY I AM TASK2  - Time 2");
	schedule(t2,t3);
	kprintf_k("HEY I AM TASK2  - Time 3");
	schedule(t2,t3);
	return;	
}
void function3()
{
	kprintf_k("HEY I AM TASK3 -- Came from TASK2 now \n");
        schedule(t3,t2);
	kprintf_k("HEY I AM TASK3 -- Tim 2\n");
        schedule(t3,t2);
	kprintf_k("HEY I AM TASK3 -- Tim 3\n"); 
	while(1);
        schedule(t3,t2);
        return;

}
//pcb *t1,*t2;
void  context_switch()
{
	t1= (pcb *)page_alloc_k(); 
	t2= (pcb *)page_alloc_k(); 
	t3= (pcb *)page_alloc_k();
		
	memset((uint64_t)t1, 0, sizeof(struct pcb));
 	memset((uint64_t)t2, 0, sizeof(struct pcb));	
	memset((uint64_t)t3, 0, sizeof(struct pcb));
	
	t1->kstack = (uint64_t*)page_alloc_k();
	t2->kstack = (uint64_t*)page_alloc_k();
	t3->kstack = (uint64_t*)page_alloc_k();

	t1->kstack[511] = (uint64_t) (&function1); 
	t2->kstack[511] = (uint64_t) (&function2); 
	t3->kstack[511] = (uint64_t) (&function3); 

	t1->rsp= (uint64_t)&(t1->kstack[511]); 
	t2->rsp= (uint64_t)&(t2->kstack[511-15]); 
	t3->rsp= (uint64_t)&(t3->kstack[511-15]);
		
	function1();
}

/*
void init_tasking()
{
	return;	
}
void switch_kernal_threads(pcb *task1, pcb *task2)
{
	regs *regs_1;
	//get flags from the task
	__asm__ volatile ("movq %%rax, %0;"
			"movq %%rbx, %1;"
			"movq %%rcx, %2;"
			"movq %%rdx, %3;"
			"movq %%rsi, %4;"
			"movq %%rdi, %5;"
			"movq %%rip, %6;"
			"movq %%rbp, %7;"
			"pushqf"
			"pop %%rax"
			"movq %%cr3, %8;"
			"movq %%rax, %9;" : regs_1->rax,regs_1->rbx,regs_1->rcx,regs_1->rdx,regs_1->rsi,regs_1->rdi,regs_1->rip,regs_1->rbp, regs_1->flags ,regs_1->cr3,regs_1->cr3: );
	task1->cpu_regs=regs_1;
	
	//now you try to save the stack pointer
	__asm__ volatile ("movq %%rsp , %0;" : task1->rsp:  );
	__asm__ volatile ("movq %0 , %%rsp;"
			  "retq;" ::&task2->rip  );
	
		


}
 
extern void initTasking();
//extern void createTask(Task*, void(*)(), uint32_t, uint32_t*);
 extern void yield(); // Switch task frontend
*/
//extern void switchTask(regs *old, regs *new); // The function which actually switches
