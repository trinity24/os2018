#include <sys/defs.h>
#include <sys/kprintf1.h>
#include <sys/task.h>
#include <sys/pmap.h>

pcb *t1,*t2;
void function1()
{
	kprintf_k("HEY I AM TASK1 -- In function task-1 now \n");
	switch_threads(t1,t2);
	kprintf("Done with it\n");
	return;
}
void function2()
{
	kprintf_k("HEY I AM TASK2 -- In function task-2 now \n");
	switch_threads(t2,t1);
	return;	
}
//pcb *t1,*t2;
void  context_switch()
{
	t1= (pcb *)page_alloc_k(); 
	t2= (pcb *)page_alloc_k(); 
		
	memset((uint64_t)t1, 0, sizeof(struct pcb));
 	memset((uint64_t)t2, 0, sizeof(struct pcb));	
	
	t1->kstack[511] = (uint64_t) (&function1); 
	t2->kstack[511] = (uint64_t) (&function2); 

	t1->rsp= (uint64_t)&(t1->kstack[511]); 
	t2->rsp= (uint64_t)&(t2->kstack[511]); 
		
	function1();
}

void switch_threads(pcb *curr, pcb *next)
{ 	
	__asm__ __volatile__(
//			  "pushq  %%rip;" 
			  "movq %%rsp, %0;"
			  "movq %1 , %%rsp;"
			  "retq;" 
                          : :"r"(&(curr->rsp)), "r"((next->rsp)));
	
	  
	return;
}
