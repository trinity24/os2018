
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

//#include <sys/process.h>

//#include <mem.h>
#define kernbase 0xffffffff80000000
#define INITIAL_STACK_SIZE 4096
//extern uint64_t pml4;
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern uint64_t kernmem, physbase,physfree;


void start(uint32_t *modulep, void *physbase, void *physfree)
{

	int  num_pages=0;uint64_t a=0,b=0;
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
			/*if (smap->base==0)
			  {
			  k=(smap->length)/page_size;
			//create_free_list(k);
			num_pages=k;
			}
			else{
			//k=((smap->base+smap->length)-( uint64_t)physfree)/page_size;
			//create_frlist(smap->base,k);
			//num_pages+=k;
			}*/
			//else num_pages=(smap->length)/page_size;
			kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
			a=smap->base;b=smap->length;			
		}
	}
	uint64_t initial_head;

	initial_head=0;
	//creating the list of free pages from the available memory. Returns the free_list head. 
	initial_head =create_list(a+b,(uint64_t)physfree);
	
	kprintf("Free_list pointer : %p \n",initial_head);
	kprintf("Available memory pages: %d \n",num_pages);
	kprintf("physfree %p and physbase %p \n", (uint64_t)physfree,(uint64_t)physbase);
	//kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	pml4 pml4_t = page_alloc();
	//uint64_t free_list_pages = num_pages* (sizeof(struct Page))/4096;
	kprintf("pml4 is %p \n",(uint64_t)pml4_t);
	

	if( pml4_t != -1)
	{
		//kprintf("base address is : %p\n", pml4_t);
		memset(pml4_t, 0, 4096);
		kernal_map(0, (uint64_t)KERNBASE+(uint64_t)physbase, (uint64_t)physbase,(uint64_t)(a+b),   pml4_t);
		identical_mapping(KERNBASE, (uint64_t)physbase,pml4_t);
//		video_map(KERNBASE+(uint64_t)0xb8000, (uint64_t)0xb8000,pml4_t);
	}


	//kprintf("%d - pages allocated \n",pages_allocated);
  pml4 kcr3 = pml4_t;
   kprintf("%p is kcr3 \n",kcr3);
  __asm__ volatile("mov %0, %%cr3"::"b"(kcr3));
    uint64_t x = page_alloc_k();
 kprintf_k("something x = %p\n ",(uint64_t)x);
  //context_switch();
  initialize_idt();
  //__asm__ volatile("sti;");
  initialise_tasks();
  create_task("bin/test");		
// switch_to_user_mode(); 
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
    
