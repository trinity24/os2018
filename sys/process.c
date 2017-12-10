//#include <sys/elf64.h>

#include <sys/process.h>
#include <sys/tarfs.h>
#include <sys/defs.h>
#include <sys/string.h>
#include <sys/task.h>
#include <sys/pmap.h>
#include <sys/elf64.h>
#include <sys/kprintf1.h>
#include <sys/gdt.h>
pcb *curr_task;
int oct2bin( char *str, int size)
{
    int n = 0;
    char *c = str;
    while (size > 0) {
        n *= 8;
        n += *c - '0';
        c++;
	size--;
    }
    return n;
}

void memcpy(void *p1,void * p2,uint64_t size)
{
        char *dst = (char *)p1;
        char *src= (char*)p2;
        while(size--)
        {
                *dst  =*src ;
		 src++;
		 dst++;
        	
	}
        return;
}
  
void switch_to_user_mode()
{
        //changing address space. Also, needs to be
	__asm__ volatile("mov %0,%%rsp;" : "=b"(curr_task->rsp));
        set_tss_rsp( (void *)curr_task->kstack+510);
	pml4 *user_pml4 = (pml4 *)page_alloc_k();
	uint64_t kernel_pml4;

	 __asm__ volatile("mov %%cr3,%0":"=b"(kernel_pml4):);

	uint64_t user_stack = page_alloc_k();
	
	page_table_walk_k(user_stack-KERNBASE,STACKTOP-4096,(pml4)user_pml4,0x7);	
        curr_task->ustack= STACKTOP;
	uint64_t start = page_alloc_k();
	vm_search(curr_task->rip);
	//page_allign
	//vm->address = 			
	//copy page tables of kernel to user pagetables.
	*(pml4 *)((uint64_t)user_pml4 + 511*(4096/512)) = *((pml4 *)(kernel_pml4+KERNBASE+511*(4096/512)));
	
 	//now load the cr3 with the user_pml4 
	__asm__ volatile("mov %0, %%cr3"::"b"((uint64_t)user_pml4-KERNBASE));
//	uint64_t user_stack = page_alloc_k();
	
        __asm__ volatile ("cli;"
                	  "pushq %1;"
        		
			  "pushq $0x23;" //push the data segmnet
                	  "pushq $0x200;"
                          "pushq $0x2B;" //cs selector
                          "pushq %0;"
			  "iretq;" //final irteq - pops eip, cs, flags then goes to cs:ip
                      
			   ::  "r"(curr_task->rip),"r"(curr_task->ustack));

}
     
//void print_task_structure(pcb *task)
int tarfs_lookup(char *tarfs,char *file,char **elf_hdr)
{
     	
	kprintf_k("%s is the filename \n",file);
	kprintf_k("here we have archive as %p \n",tarfs);
	char *p= tarfs;    
    while (!Mystrcmp(p + 257, "ustar", 5)) 
    {
        int filesize = oct2bin(p+0x7c,11);
        if (!Mystrcmp(p,file,Mystrlen(file)+1)) 
	{
            *elf_hdr = p+ 512;
            return filesize;
        }
        p+=(((filesize+511) /512)+1)*512;
    }
    return 0;
}

void elf_read(Elf64_Ehdr *elf,vm_struct **mmap)
{
	
	uint64_t phdr_offset = elf->e_phoff;
	phdr_offset  = (uint64_t)elf + phdr_offset;
	Elf64_Phdr* phdr;
	phdr = (Elf64_Phdr *)phdr_offset;
	//vm_struct temp;
	int num_phdr = elf->e_phnum;	
	kprintf_k("Now i try to print the values of the program headers for memory ranges\n");	
	
	vm_struct *list=NULL;
	for(int i=0;i<num_phdr;i++)
	{
		if(phdr->p_type==1)
		{
			vm_struct *temp =(vm_struct *)page_alloc_k();
	                temp->flags= phdr->p_flags;
        	        temp->address = (phdr->p_vaddr);
                	temp->size = phdr->p_memsz;
            	        temp->next= NULL;
	
			temp->fp = (uint64_t)elf+ (uint64_t)(phdr->p_offset);
			temp->filesize = phdr->p_filesz; 				
			temp->type=0;
			if(list==NULL)
                        	list= temp;
                	else
                        {
				temp->next=list;
				list=temp;
			}
		}
			
		
		phdr++;
	}
	
	vm_struct *heap= (vm_struct *)page_alloc_k();
	heap->address= KERNBASE -((4096)*1000);
	heap->type= 2;
	heap->size = 4096;
	if(!list)
	{
		heap->next =list;
		list=heap;
	}
	
	vm_struct *stack = (vm_struct *)page_alloc_k();//;KERNBASE - (4096)*1000;
	stack->address = 0xfffffe0000000000;
	stack->type =1;
	stack->size = 4096;
	        if(!list)
        {
                stack->next =list;
                list=stack;
                }
	*mmap=list;
	return;
}

uint64_t elf_load(char *filename,vm_struct **list)
{
	char *elf_hdr;
	int filesize;
	filesize = tarfs_lookup(&(_binary_tarfs_start),filename,&elf_hdr );
	kprintf_k("%d is the size of the file \n",filesize);
	Elf64_Ehdr *elf =(Elf64_Ehdr *)elf_hdr;
	elf_read( (Elf64_Ehdr *)elf , list);	
	return elf->e_entry;

}
/*
void print_task_structurepcb *task)
{
	kprintf_k("This is to print the task structure \n ");
	return;
}*/
void create_task(char *filename)
{
	
	pcb *task =(pcb *) page_alloc_k();
	memset((uint64_t)task, 0, sizeof(struct pcb));
	task->kstack= (uint64_t *)page_alloc_k();
	//memset((uint64_t)task, 0, sizeof(struct pcb));
	task->vm_head=(vm_struct *) page_alloc_k();
      	//memset((uint64_t)task->mm, 0, sizeof(struct mm_struct));
	task->next_task=NULL;
	task->ustack =  0xfffffe0000000000;
//      	task->prev_task=NULL; 	
	task->parent=NULL:
	task->child=NULL;
	kpirintf_k("%s  is the filename in createtask \n",filename);		
	vm_struct *list=NULL;
	
	task->rip = elf_load(filename,&list);
	
	task->vm_head =list;
	curr_task= task;
//	print_task_structure(pcb *task);
	switch_to_user_mode();	
	return;	
}
vm_struct *vm;
uint64_t vm_search(uint64_t addr)
{
	//vm = (vm_struct *)
	vm =curr_task->mm->mmap;
	while(vm)
	{	
		uint64_t start = vm->address;
		uint64_t end = start+ vm->size;
		 
		if(addr>=start &&addr<=end)
			return 1;
		
		vm++;
		
	}
	return 0;
}
void page_fault_handler()
{
	int cow = copy_on_write();
	if (!cow)
	{
		// This code has to be changed. Reemmeber
		uint64_t va= page_alloc_k();
        	pml4 cr3;
        	uint64_t cr2;
        	__asm__ volatile("mov %%cr3,%0":"=b"(cr3):);
        	__asm__ volatile("mov %%cr2,%0":"=b"(cr2):);
        	uint64_t pa= get_paddr(va,cr3);
        	page_table_walk_k(pa,cr2&PAGEALIGN,cr3,0x07);
        	uint64_t file_fp, file_size;
	
        	if(vm_search(cr2))
        	{
                 	file_fp = vm->address;
                	 file_size = vm->size;
                 	//memcpy((void *)cr2,(void *)file_fp,(uint64_t)file_size);
        	}
	}
	
	return;
}
