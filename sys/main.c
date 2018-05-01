
#include <sys/defs.h>
#include <sys/process.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/kprintf1.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/pci.h>
#include <sys/idt.h>
#include <sys/pmap.h>
#include <sys/pic.h>
#include <sys/task.h>
#include <sys/mem.h>
#define kernbase 0xffffffff80000000
#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern uint64_t kernmem, physbase,physfree;

void start(uint32_t *modulep, void *physbase, void *physfree)
{

	print_initial_state((uint64_t)physbase, (uint64_t)physfree);
	uint64_t avail_mem_end= get_avail_mem_end(modulep);
	initialise_memory_management(modulep,(uint64_t)physfree,avail_mem_end);
	print_free_list_head();
	initialise_paging((uint64_t)physbase, (uint64_t)physfree,avail_mem_end);
 	initialize_idt();
   	initialise_tasks();
	__asm__ volatile("sti");
	//create task, should accept argumets TODO: execve should accept arguments
   	create_task("bin/test");		
   	while(1); 
 	return;
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__(
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );

  kprintf("%s\n",(char*)(uint64_t)loader_stack[3]);
  kprintf("%s\n",(char*)(uint64_t)loader_stack[4]);
  init_gdt();
//  interrupt_call(32);
	initialize_idt();  
start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1);
}
    
