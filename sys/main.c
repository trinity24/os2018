#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/pci.h>
#include <sys/idt.h>
#include <sys/pmap.h>
#include <sys/pic.h>
//#include <mem.h>
#define kernbase 0xffffffff80000000
#define INITIAL_STACK_SIZE 4096
//extern uint64_t pml4;
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern uint64_t kernmem, physbase,physfree;
//extern uint64_t pml4;
//void *kernbase, *kernfree;
void start(uint32_t *modulep, void *physbase, void *physfree)
{
	
	int  num_pages=0;
    struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
	
//	uint32_t num_pages=0;  
while(modulep[0] != 0x9001) modulep += modulep[1]+2;
   uint32_t base1;
	
	uint64_t a=0,b=0;
//   uint32_t page_size=4096;
	
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
	kprintf("%p\n",a+b);
	
	num_pages=create_list(a+b,(uint64_t)physfree);
	//kprintf("before this`");	
	kprintf("Available memory pages: %d\n",num_pages);
  kprintf("physfree %p and physbase %p\n", (uint64_t)physfree,(uint64_t)physbase);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  uint64_t pml4 = page_alloc();
 kprintf("pml4 is %p \n",(uint64_t)pml4); 
  if(pml4!=-1)
  {
 	 memset((void *)pml4, 0, 512);
  	 
 	 kernal_map(kernbase, (uint64_t)physbase,pml4);
 // pa_to_va_map(kernbase, physbase);
  
  }
  
  initialize_idt();

  PIC_remap(32,40);
 // __asm__("sti");

     uint8_t bus;
     uint8_t device;
     for(bus = 0; bus < 32; bus++) {
         for(device = 0; device < 255; device++) {
            if( checkAHCI(bus, device)){
		goto x;
		}
		
         }
     }
x:
//	uint32_t base1;
	base1=get_AHCIBASE(bus,device);
	ahci(base1);	
 
while(1)
{
	
}
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
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
	clearscreen();
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1);
}
