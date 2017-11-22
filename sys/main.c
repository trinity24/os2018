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
#include <sys/task.h>
//#include <mem.h>
#define kernbase 0xffffffff80000000
#define INITIAL_STACK_SIZE 4096
//extern uint64_t pml4;
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern uint64_t kernmem, physbase,physfree;
void k_thread1();
void k_thread2();
uint64_t *currsp;
//extern uint64_t pml4;
//void *kernbase, *kernfree;

void k_thread3(){
  return;
}

void k_thread1(){
  void (*fun_ptr)() = &k_thread2;
  void (*fun_ptr2)() = &k_thread1;
  pcb *p1 = (pcb *)page_alloc();
  pcb *p2 = (pcb *)page_alloc();
  memset((uint64_t)p1, 0, sizeof(struct pcb));
  memset((uint64_t)p2, 0, sizeof(struct pcb));
  p1->kstack[127] = (uint64_t)fun_ptr;
  p2->kstack[127] = (uint64_t)fun_ptr2;
  uint64_t *currsp = &(p2->kstack[127]);
  __asm__ volatile(
    "\
      movq %0, %%rsp;\
      pushq %%rax; \
      pushq %%rbx; \
      pushq %%rcx; \
      pushq %%rdx; \
      pushq %%rsi; \
      pushq %%rdi; \
      pushq %%rbp; \
      movq %0, %%rax;\
    "
    :
    :"m" (currsp)
    : "%rsp"
  );
  kprintf("Hello world\n");
  uint64_t *sp = &(p1->kstack[121]);
  kprintf(" value of currsp is : %p\n", currsp);
  __asm__ volatile(
    "\
      movq %0, %%rsp;\
      popq %%rbp; \
      popq %%rdi; \
      popq %%rsi; \
      popq %%rdx; \
      popq %%rcx; \
      popq %%rbx; \
      ret\
    "
    :
    :"m" (sp)
    : "%rsp"
  );
  //kprintf("currsp after pushing %p\n", currsp);
  kprintf("nextsp %p\n", sp);
}

/*pushq %%rax; \
pushq %%rbx; \
pushq %%rcx; \
pushq %%rdx; \
pushq %%rsi; \
pushq %%rdi; \
pushq %%rbp; \*/

/*popq %%rbp; \
popq %%rdi; \
popq %%rsi; \
popq %%rdx; \
popq %%rcx; \
popq %%rbx; \
popq %%rax; \*/

void k_thread2(){
  kprintf("In kthread2\n");
  __asm__ volatile(
    "\
      pushq %%rax; \
      pushq %%rbx; \
      pushq %%rcx; \
      pushq %%rdx; \
      pushq %%rsi; \
      pushq %%rdi; \
      pushq %%rbp; \
    "
    ::
  );
  uint64_t *sp;
  __asm__ volatile(
    "\
      movq %%rax, %0;\
    "
    : "=m" (currsp)
    :
  );
  kprintf("currsp is %p\n", currsp);
  __asm__ volatile(
    "\
      movq %%rsp, %0;\
      movq %1, %%rsp;\
      popq %%rbp; \
      popq %%rdi; \
      popq %%rsi; \
      popq %%rdx; \
      popq %%rcx; \
      popq %%rbx; \
      popq %%rax; \
      ret\
    "
    :"=m" (sp)
    :"m" (currsp)
    : "%rsp"
  );
  while(1);
}


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
  pdp pdp_b = *((pml4 *)pml4_t + pml4_i) & 0xfffffffffffff000;
  pd pd_b = *((pdp *)pdp_b + pdpe_i) & 0xfffffffffffff000;
  pt pt_b = *((pd *)pd_b + pde_i) & 0xfffffffffffff000;
  uint64_t pya = *((pt *)pt_b + pte_i) & 0xfffffffffffff000;
	kprintf("va : %p to pa : %p\n", va, pya+extract_bits_frm_va(va, 0));
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
  kprintf("The space taken will be : %d\n", ((a+b)/4096) * sizeof(struct Page) / 4096);
	num_pages=create_list(a+b,(uint64_t)physfree);
  c_n_l(a+b,(uint64_t)physfree);
  //create_list_n(a+b,(uint64_t)physfree);
	//kprintf("before this`");
	kprintf("Available memory pages: %d\n",num_pages);
  kprintf("physfree %p and physbase %p\n", (uint64_t)physfree,(uint64_t)physbase);
  //kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  pml4 pml4_t = page_alloc();
  //pml4_t = page_alloc();
 	//kprintf("pml4 is %p \n",(uint64_t)pml4);
  if( pml4_t != -1)
  {
    //kprintf("base address is : %p\n", pml4_t);
 	  memset(pml4_t, 0, 4096);
    //kprintf("value at %p is %d\n", pml4_t, *((char *)pml4_t));
 	  kernal_map(kernbase+(uint64_t)physbase, (uint64_t)physbase, pml4_t);
 	  video_map(kernbase+(uint64_t)0xb8000, (uint64_t)0xb8000,pml4_t);
  }
  //uint64_t lh= get_free_list_head();
  map_va_to_pa(kernbase+(uint64_t)physfree, (uint64_t)physfree,pml4_t);
  k_thread1();
  kprintf("Hello world\n");
  //k_thread1();
 	//va_to_pa(kernbase+(uint64_t) 0x201233, (uint64_t) 0x201233, pml4_t);
  //pml4 kcr3 = pml4_t;
	//__asm__ volatile("mov %0, %%cr3"::"b"(kcr3));
  //pml4 x = page_alloc();
  //kprintf("x %p\n", x);


  while(1);
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
