
#include <sys/process.h>
#include <sys/pmap.h>
#define PAGEALIGN 0xfffffffffffff000

void copy_pagetables(uint64_t pml4_p)
{
	pml4_p= (pml4 * )(pml4_p);
	pml4_c= (pml4 *)page_alloc_k();
	memset(pml4_c, 0,PAGESIZE)
	for(int i=0;i<512;i++)
	{
		if(*(pml4_p +i*sizeof(uint64_t))&3 )
                {
			pml4_c+i*sizeof(uint64_t) = (pdp *)page_alloc_k();
			pdp *pdp_p = pml4_p +i*sizeof(uint64_t);
					
                	pdp *pdp_c =pml4_c+i*sizeof(uint64_t);
			for(int j=0;j<512;j++)
			{
				if(*(pdp_p +j*sizeof(uint64_t))&3 )	
				{
					pdp_c+j*sizeof(uint64_t) = (pd *)page_alloc_k();
                        		pd *pd_p = pdp_p +j*sizeof(uint64_t);
					pd *pd_c = pdp_c+j*sizeof(uint64_t);
					for(int k=0;k<512;k++)
					{
						if(*(pd_p +k*sizeof(uint64_t))&3 )
                                		{
                                        		pd_c+k*sizeof(uint64_t) = (pt *)page_alloc_k();
                                        		pt *pt_p = pd_p +k*sizeof(uint64_t);
							pt *pt_c = pd_c+k*sizeof(uint64_t);
							for(int l=0;l<512;l++)
							{
								//pt_c+l*sizeof(uint64_t) = (pt *)page_alloc_k();
								*(pt_c +l*sizeof(uint64_t) )= *(pt_p +l*sizeof(uint64_t)) &5 ;
							
							}
						}
					}
	
				}
			}

		}
		
	}
	return;
		// when he present bit is present
}


void copy_on_write()
{	
	uint64_t flag;
	__asm__ volatile ("pop %%eax" 
			  "subq %%eax, $0x7"	
			  "movq %%eax ,%0", : flag:);
	if (flag)
	{
		// since errocode is 7., The exception is regarding COW 
		//Now, we execute COW.
		//create a new page and copy the content of the current page to the new page allocated.
		uint64_t cr2;
		uint64_t page = page_alloc_k();
		__asm__ volatile("mov %%cr2,%0":"=b"(cr2):);
        	uint64_t pa= cr2-KERNBASE;
		
		page_table_walk_k(pa, page,*(curr_task->pml4) ,7);
		memcpy( page, cr2&PAGEALIGN,PAGESIZE );
		return 1;
	}
	
	return 0;
}
void copy_vmas (vm_struct *parent, vm_struct **child)
{
	vm_struct *temp =parent,*prev;
	while(temp)
	{
		
		vm_struct *vm  = (vm_struct *)page_alloc_k();
		memset(vm, 0,PAGESIZE);
		vm->address = temp->address;
		vm->size= temp->size;
		vm->type =temp->type;
		vm->fp = vm->fp;
		vm->filesizel;
		if(temp==parent)
		{
			vm->next=NULL;
			child = vm;
			prev = vm;
		}
		else
		{
			prev->next=vm;
			vm->next=NULL;
		
		}
		temp++;
	}
	return;
	
}

void fork(pcb *parent)
{
	
	pcb *child= page_alloc_k();
        child->pid = pid++;
	child->kstack = page_alloc_k();
	memcpy(child->kstack, parent->kstack, PAGESIZE);
        
	child->rsp=parent->rsp;
	
	//what should be the state
	copy_pagetables(pml4);
	child->pid_parent = parent->pid;
	

	//child->mm = (mm_struct *)page_alloc_k();		
	vm_struct *list = page_alloc_k();
	memset(list,0,PAGESIZE);
	copy_vmas(parent->mmstruct, &list);	
	child->mmstruct = list;
	
	//how to give next and prev processes? 
	child->prev_task = NULL;
	child->next_task = NULL;
	 
	
	
 	return;


}
