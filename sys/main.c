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

uint64_t extract_bits_frm_va(uint64_t virtual_address, int start)
{
  // create number of ones,
	  //uint64_t mask= (1<<(start-end+1));
		  //return (virtual_address>>start)&mask;
  if(start  == 39)
    return (virtual_address >> 39) & 0x1FF;
  else if(start == 30)
    return (virtual_address >> 30) & 0x1FF;
  else if(start == 21)
    return (virtual_address >> 21) & 0x1FF;
  else if(start == 12)
   return (virtual_address >> 12) & 0x1FF;
  else
   return (virtual_address) & 0xFFF;
}

void va_to_pa(uint64_t va, uint64_t pa, pml4 pml4_t){
	uint64_t pml4_i = extract_bits_frm_va(va, 39);
	uint64_t pdpe_i = extract_bits_frm_va(va, 30);
	uint64_t pde_i = extract_bits_frm_va(va, 21);
	uint64_t pte_i = extract_bits_frm_va(va, 12);
	// uint64_t pml4_e = (*((uint64_t *)(pml4a + pml4_i))) >> 12;
	// uint64_t pdp_e = (*((uint64_t *)(pml4_e + pdpe_i))) >> 12;
	// uint64_t pd_e = (*((uint64_t *)(pdp_e + pde_i))) >> 12;
	// uint64_t pt_e = *((uint64_t *)pd_e + pte_i) >> 12;
  pdp pdp_b = *((pml4 *)pml4_t + pml4_i) >> 12;
  pd pd_b = *((pdp *)pdp_b + pdpe_i) >> 12;
  pt pt_b = *((pd *)pd_b + pde_i) >> 12;
  uint64_t pya = *((pt *)pt_b + pte_i) >> 12;
	kprintf("va : %p to pa : %p\n", va, pya);
	//kprintf("%p %p %p %p\n", pml4_e, pdp_e, pd_e, pt_e + extract_bits_frm_va(va, 0));
}

void start(uint32_t *modulep, void *physbase, void *physfree)
{

	int  num_pages=0;
    struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;

	initScreen();
//	uint32_t num_pages=0;
while(modulep[0] != 0x9001) modulep += modulep[1]+2;
   //uint32_t base1;

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
  //kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  pml4 pml4_t = page_alloc();
  //pml4_t = page_alloc();
 	//kprintf("pml4 is %p \n",(uint64_t)pml4);
  if( pml4_t != -1)
  {
    kprintf("base address is : %p\n", pml4_t);
 	  memset(pml4_t, 0, 4096);
    kprintf("value at %p is %d\n", pml4_t, *((char *)pml4_t));
 	  kernal_map(kernbase+(uint64_t)physbase, (uint64_t)physbase, pml4_t);
 	  video_map(kernbase+(uint64_t)0xb8000, (uint64_t)0xb8000,pml4_t);
  }
	//kprintf("loading address %p in cr3\n", *pml4_t);
	//kprintf("address : %p", *(((uint64_t *)0x212000)) + 0x1);
	//for(uint64_t k = 0; k < 0x000; k+=0x1000)
	va_to_pa(kernbase+(uint64_t) physbase, (uint64_t) physbase, pml4_t);
 	//va_to_pa(kernbase+(uint64_t) 0x201233, (uint64_t) 0x201233, pml4_t);
  pml4 kcr3 = pml4_t << 12;
  kprintf("loading %p\n", kcr3);
	__asm__ volatile("mov %0, %%cr3"::"b"(kcr3));
  //initialize_idt();

  //PIC_remap(32,40);
 // __asm__("sti");

    // uint8_t bus;
     //uint8_t device;
    // for(bus = 0; bus < 32; bus++) {
    //     for(device = 0; device < 255; device++) {
    //        if( checkAHCI(bus, device)){
		//goto x;
		//}

      //   }
     //}
//x:
//	uint32_t base1;
//	base1=get_AHCIBASE(bus,device);
//	ahci(base1);

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
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1);
}
