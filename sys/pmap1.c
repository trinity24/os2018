//#include<stdio.h>
#include <sys/defs.h>
#include <sys/pmap.h>
#include <sys/kprintf.h>
#include <stdlib.h>
//#include <string.h>


struct Page *free_list;


int create_list( uint64_t end)
{
	int p=0;
	//	struct Page *prev=NULL;
	uint64_t paddr=0;
	struct Page *pages=(struct Page *)(PHYSFREE);
	int max= end/4096;
	kprintf("coming into this with end as %p\n",end);
	for(int i=0;i<max;i++)
	{
		p++;	
		//(struct Page *)
		(pages+i*(sizeof(struct Page)))->id = paddr+i*PAGESIZE;
		if( paddr+i*PAGESIZE<PHYSFREE )
		{
			//(struct Page *)
			(pages+i*(sizeof(struct Page)))->status = ALLOCATED;
			//(struct Page *)
			(pages+i*(sizeof(struct Page)))->next = NULL;	
		}
		else
		{
	/*
			if(paddr + i*PAGESIZE == PHYSFREE)
			{
				(struct Page *)(pages+i*(sizeof(struct Page)))->next =NULL;
				free_list=pages+i*(sizeof(struct Page));
			}	
			*/
			//prev=free_list;
			//(struct Page )*
			(pages+i*(sizeof(struct Page)))->next =free_list;
			//(struct Page )*
			(pages+i*(sizeof(struct Page)))->id =paddr+i*PAGESIZE;
			//(struct Page )*
			(pages+i*(sizeof(struct Page)))->status = FREE;
			free_list= (struct Page *)(pages+i*(sizeof(struct Page)));
			
		}	
	}
	return p;
	//kprintf("It came out of this \n");
}

uint64_t page_alloc()
{
	uint64_t addr;
	if(!free_list)
	{
		addr=free_list->id;
		free_list->status=1;
		struct Page *temp= free_list->next;
		free_list->next=NULL;
		free_list=temp;
		return addr;
	}
	/*	(struct Page *)
	free_list->status = 1;
	//struct Page *temp= free_list;
	free_list->next=NULL;
	free_list=temp;
	temp->next=NULL;
	return temp->id;*/
	else{
		kprintf("Pages not available, ran out of memory !!\n");
		return (uint64_t)(-1);
	}	
	
}

int is_free(int n)
{
	if(((struct Page *)(PHYSFREE +(uint64_t) n))->status==1)
		return 0;
	else
		return 1;
}

void set_page_free(int n)
{
	if((((struct Page *)(PHYSFREE +(uint64_t) n)))->status==1)
		{
			((struct Page *)(PHYSFREE + (uint64_t)n))->status=0;
			struct Page *temp =((struct Page *)(PHYSFREE + (uint64_t)(n)))->next = free_list;
			free_list = temp;
		}
	return;
}

uint64_t extract_bits_from_va(uint64_t virtual_address, int start, int end)
{
	// create number of ones, 
	uint64_t mask= (1<<(start-end+1));
	return (virtual_address>>start)&mask;  

}

void kern_map(uint64_t kernbase, uint64_t physbase)
{

	int num_pages= (PHYSFREE - PHYSBASE)/4096;
	uint64_t va,pa;
	for(int i=0;i<num_pages;i++)
	{
		va=kernbase+i*(PAGESIZE);
		pa=physbase+i*(PAGESIZE);
		page_table_walk(pa,va);
	}
	return;
}
void page_table_walk(uint64_t pa, uint64_t va)
{
	 uint64_t virtual_address=va;
	uint64_t pml4_index = extract_bits_from_va(virtual_address,39,47);
	if( ( *  ( (uint64_t*) (pml4 +pml4_index) ) )& 01 )	
	{
		//if pml4 entry for the index is present
		
		uint64_t pdp = *(  ( (uint64_t*) (pml4 + pml4_index) ) ); //(((uint64_t*))(pml4 + pml4_index));
		uint64_t pdpt_index = extract_bits_from_va(virtual_address,30,38);

		if((*((uint64_t*)(pdp+pdpt_index)))&01)
		{
			uint64_t pd = *((uint64_t*)(pdp+pdpt_index));
		  	uint64_t pde_index = extract_bits_from_va(virtual_address,21,29);

			if((*((uint64_t *)(pd +pdpt_index)))&01)
			{
				uint64_t pt = *((uint64_t*)(pd+pde_index));
				uint64_t pte_index = extract_bits_from_va(virtual_address,12,20);
				*((uint64_t*)(pt+pte_index))  = pa;

			}
			else
			{
				uint64_t pt= page_alloc();
				memset(pt, 0, 512);
				*((uint64_t*)(pd+pde_index))= (pt|3);
		
				uint64_t pte_index = extract_bits_from_va(virtual_address,12,20);
				*((uint64_t*)(pt+pte_index))  = pa;
			}

		}
		else
		{
			uint64_t pd= page_alloc();
			memset(pd, 0, 512);
			*((uint64_t*)(pdp+pdpt_index)) = pd|3;

			uint64_t pde_index = extract_bits_from_va(virtual_address,21,29);
			uint64_t pt= page_alloc();
			memset(pt, 0, 512);
			*((uint64_t*)(pd+pde_index)) = pt|3;
		
			uint64_t pte_index = extract_bits_from_va(virtual_address,12,20);
			*((uint64_t*)(pt+pte_index)) = pa;
		}
	}
	else
	{
		//make a pagedirectory pounter table;
		//fill in the pml4 entry with that
		uint64_t pdp=page_alloc();
		memset(pdp, 0, 512);
		*((uint64_t*)(pml4 +pml4_index)) = pdp|3;
		//now fill in the pdpt entry. 
		uint64_t pdpt_index = extract_bits_from_va(virtual_address,30,38);
		
		//:187pdp_thread:
		uint64_t pd= page_alloc();
		memset(pd, 0, 512);
		*((uint64_t*)(pdp+pdpt_index)) = pd|3;

		uint64_t pde_index = extract_bits_from_va(virtual_address,21,29);
		 uint64_t pt= page_alloc();
		memset(pt, 0, 512);
		*((uint64_t *)(pd+pde_index))= pt|3;
		
		
		uint64_t pte_index = extract_bits_from_va(virtual_address,12,20);
		*((uint64_t*)(pt+pte_index))  = pa;
			
	
	}
	return;

}
/*
void page_table_walk(uint64_t pa, uint64_t va)
{
	uint64_t virtual_address =va;
	uint64_t pml4_index = extract_bits_from_va(virtual_address,39,47);
	kprintf("%d",pml4_index);
	
	if((pml4 *)pml4->pml4_entries[pml4_index]&0x01)
	{
		//if pml4 entry for the index is present
		
		uint64_t pdp = pml4->pml4_entries[pml4_index];
		uint64_t pdpt_index = extract_bits_from_va(virtual_address,30,38);
		
		if((pdp *)pdp->pdp_entries[pdpt_index]&01)
		{
			uint64_t pd = pdp[pdpt_index];
		  	uint64_t pde_index = extract_bits_from_va(virtual_address,21,29);

			if((pd *)pd->pd_entries[pdpt_index]&01)
			{
				uint64_t pt = pd[pde_index];
				uint64_t pte_index = extract_bits_from_va(virtual_address,12,20);
				(pd *)pt->pt_entries[pte_index]  = pa;

			}
			else
			{
				uint64_t pt= page_alloc();
				memset(pt, 0, 512);
				(pd *)pd->pd_entries[pde_index]= pt|3;
		
				uint64_t pte_index = extract_bits_from_va(virtual_address,12,20);
				(pt *)pt->pt_entries[pte_index]  = pa;
			}

		}
		else
		{
			uint64_t pd= page_alloc();
			memset(pd, 0, 512);
			((pdp *)pdp)->pdp_entries[pdpt_index] = pd|3;

			uint64_t pde_index = extract_bits_from_va(virtual_address,21,29);
			uint64_t pt= page_alloc();
			memset(pt, 0, 512);
			(pd *)pd->pd_entries[pde_index]= pt|3;
		
			uint64_t pte_index = extract_bits_from_va(virtual_address,12,20);
			(pt *)pt->pt_entries[pte_index]  = pa;

		}
	}
	else
	{
		//make a pagedirectory pounter table;
		//fill in the pml4 entry with that
		uint64_t pdp=page_alloc();
		memset(pdp, 0, 512);
		((pml4 *)pml4)->pml4_entries[pml4_index] = pdp|3;
		//now fill in the pdpt entry. 
		uint64_t pdpt_index = extract_bits_from_va(virtual_address,30,38);
		kprintf("%d",pdpt_index);
		
		uint64_t pd= page_alloc();
		memset(pd, 0, 512);
		
		((pdp *)pdp)->pdp_entries+pdpt_index = pd|3;

		uint64_t pde_index = extract_bits_from_va(virtual_address,21,29);
		kprintf("%d",pde_index);
		uint64_t pt= page_alloc();
		memset(pt, 0, 512);
		((pd *)pd)->pd_entries[pde_index]= pt|3;
		
		
		uint64_t pte_index = extract_bits_from_va(virtual_address,12,20);
		kprintf("%d",pte_index);
		((pt *)pt)->pt_entries[pte_index]  = pa;
	}
	
	
	return;

}*/
