//#include<stdio.h>
#include <sys/defs.h>
#include <sys/pmap.h>
#include <sys/kprintf.h>
#include <stdlib.h>
//#include <string.h>




int create_list( uint64_t end, uint64_t physfree)
{
//	kprintf("Free list at the start of create_list is %p \n",free_list);

	uint64_t  p=0,f=0;
	//	struct Page *prev=NULL;
	uint64_t paddr=0;
	struct Page *pages=(struct Page *)(physfree);
	uint64_t max= end/4096;
	struct Page *temp;
//	uint64_t size_of_list =((sizeof(struct Page))*(availablememory)/4096);
	//kprintf("coming into this with end as %p\n",end);
	for(uint64_t i=0;i<max-1;i++)
	{
		p++;
		//(struct Page *)
		(pages+i*(sizeof(struct Page)))->id = paddr+i*PAGESIZE;
		if( paddr+i*PAGESIZE<(physfree+3*(4096)) ){
			//(struct Page *)
			(pages+i*(sizeof(struct Page)))->status = ALLOCATED;
			//(struct Page *)
			(pages+i*(sizeof(struct Page)))->next = NULL;
		}
		else
		{
//			kprintf("%d is the f \n",f);
	/*
			if(paddr + i*PAGESIZE == PHYSFREE)
			{
				(struct Page *)(pages+i*(sizeof(struct Page)))->next =NULL;
		*/	if(f==0)
			{

				free_list= (pages+i*(sizeof(struct Page)));
				(pages+i*(sizeof(struct Page)))->status = FREE;
				(pages+i*(sizeof(struct Page)))->id =paddr+i*PAGESIZE;
//				kprintf("the address of free_list page is %p \n ",free_list->id);
				free_list->next=NULL;
				temp=free_list;
		//
				f++;
	//			kprintf("%p is the first time free_list is updated \n",free_list);
			}
			else
			{
				temp->next = (pages+i*(sizeof(struct Page)));
				//(struct Page )*
				(pages+i*(sizeof(struct Page)))->id =paddr+i*PAGESIZE;
			//(struct Page )*
				(pages+i*(sizeof(struct Page)))->status = FREE;
				(pages+i*(sizeof(struct Page)))->next=NULL;
				temp=(pages+i*(sizeof(struct Page)));
				f++;
			}
		}
	}
	//kprintf("%d is the number of free_pages\n",f);

	kprintf("Free list at the end of create_list is %p \n",free_list);
	return p;

}

// void memset(void *mem, int val, int len)
// {
//   //int i;
//   uint64_t *p =(uint64_t *)  mem;
// //  i = 0;
//   while(len > 0)
//     {
//       *p = (uint64_t)val;
//       p++;
//       len--;
//     }
//   return;
// }

void memset(uint64_t mem, int val, int len){
	char *p = NULL;
	p = (char *)mem;
	while(len > 0){
	 	*p = val;
	 	p++;
	 	len--;
	 }
	return;
}


uint64_t page_alloc()
{
	uint64_t addr;
	if(free_list!=NULL)
	{
		addr=free_list->id;
		free_list->status=1;
		struct Page *temp= free_list->next;
		free_list->next=NULL;
		free_list=temp;
//		kprintf("free_list now is- after allocating page %p",free_list);
		return addr;
	}
	else{
		kprintf("Pages not available, ran out of memory !!\n");
		return -1;
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

uint64_t extract_bits_from_va(uint64_t virtual_address, int start)
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
	else
		return (virtual_address >> 12) & 0x1FF;

}

void kernal_map(uint64_t kernbase, uint64_t physbase,pml4 pml4_t)
{

	int num_pages= (PHYSFREE - PHYSBASE)/4096;
	uint64_t va,pa;
	kprintf("%d\n", (PHYSFREE-PHYSBASE) /4096);
	kprintf("pml4 %p\n", pml4_t);
	for(int i=0;i<num_pages;i++)
	{
		va=kernbase+i*(PAGESIZE);
		pa=physbase+i*(PAGESIZE);
		page_table_walk(pa,va,pml4_t);
	}
	return;
}
void video_map(uint64_t vm_va, uint64_t vmbase,pml4 pml4_t)
{
  page_table_walk(vmbase,vm_va,pml4_t);
	page_table_walk(vmbase+1*(PAGESIZE),vm_va+1*(PAGESIZE),pml4_t);
	page_table_walk(vmbase+2*(PAGESIZE),vm_va+2*(PAGESIZE),pml4_t);
  return;
}
void page_table_walk(uint64_t pa, uint64_t va,pml4 pml4_t)
{
	uint64_t pml4_i = extract_bits_from_va(va, 39);
	uint64_t pdpe_i = extract_bits_from_va(va, 30);
	uint64_t pde_i = extract_bits_from_va(va, 21);
	uint64_t pte_i = extract_bits_from_va(va, 12);
	if( *((pml4 *)pml4_t + pml4_i) & 0x3){
		//pdp pdp_t = *((pml4 *)pml4_t + pml4_i) >> 12;
		pdp pdp_t = *((pml4 *)pml4_t + pml4_i) & 0xfffffffffffff000;
		if( *((pdp *)pdp_t + pdpe_i) & 0x3){
			//pd pd_t = *((pdp *)pdp_t + pdpe_i) >> 12;
			pd pd_t = *((pdp *)pdp_t + pdpe_i) & 0xfffffffffffff000;
			if(*((pd *)pd_t + pde_i) & 0x3){
					//pt pt_t = *((pd *)pd_t + pde_i) >> 12;
					pt pt_t = *((pd *)pd_t + pde_i) & 0xfffffffffffff000;
					*((pt *)pt_t + pte_i) = pa | 0x3;
					//kprintf("%p %p %p %p %p\n", pml4_t, *((pml4 *)pml4_t + pml4_i), *((pdp *)pdp_t + pdpe_i), *((pd *)pd_t + pde_i), *((pt *)pt_t + pte_i));
			}else{
				//create pt
				pt pt_t = page_alloc();
				if(pt_t != -1){
					memset(pt_t, 0, 4096);
					*((pd *)pd_t + pde_i) = pt_t | 0x3;

					*((pt *)pt_t + pte_i) = pa | 0x3;
					//kprintf("%p %p %p %p %p\n", pml4_t, *((pml4 *)pml4_t + pml4_i), *((pdp *)pdp_t + pdpe_i), *((pd *)pd_t + pde_i), *((pt *)pt_t + pte_i));
				}else{
					kprintf("Ran out of free memory\n");
				}
			}

		}else{
			//create pd
			pd pd_t = page_alloc();
			if(pd_t != -1){
				memset(pd_t, 0, 4096);
				pt pt_t = page_alloc();
				if(pt_t != -1){
					memset(pt_t, 0, 4096);
					*((pd *)pd_t + pde_i) = pt_t | 0x3;

					*((pt *)pt_t + pte_i) = pa | 0x3;
					//kprintf("%p %p %p %p %p\n", pml4_t, *((pml4 *)pml4_t + pml4_i), *((pdp *)pdp_t + pdpe_i), *((pd *)pd_t + pde_i), *((pt *)pt_t + pte_i));
				}else{
					kprintf("Ran out of free memory\n");
				}
			}
			else{
				kprintf("Ran out of free memory\n");
			}

		}
	}else{
		pdp pdp_t = page_alloc();
		if(pdp_t != -1){
			memset(pdp_t, 0, 4096);
			//*((pml4 *)pml4_t + pml4_i) = (pdp_t << 12) | 0x3;
			*((pml4 *)pml4_t + pml4_i) = pdp_t| 0x3;

			pd pd_t = page_alloc();
			if(pd_t != -1){
				memset(pd_t, 0, 4096);
				//*((pdp *)pdp_t + pdpe_i) = (pd_t << 12) | 0x3;
				*((pdp *)pdp_t + pdpe_i) = pd_t | 0x3;

				pt pt_t = page_alloc();
				if(pt_t != -1){
					memset(pt_t, 0, 4096);
					*((pd *)pd_t + pde_i) = pt_t| 0x3;

					*((pt *)pt_t + pte_i) = pa | 0x3;
					//kprintf("%p %p %p %p %p\n", pml4_t, *((pml4 *)pml4_t + pml4_i), *((pdp *)pdp_t + pdpe_i), *((pd *)pd_t + pde_i), *((pt *)pt_t + pte_i));
				}else{
					kprintf("Ran out of free memory\n");
				}
			}else{
					kprintf("Ran out of free memory\n");
			}
		}else{
			kprintf("Ran out of free memory\n");
		}
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
				memset(pt, 0, 4096);
				(pd *)pd->pd_entries[pde_index]= pt|3;

				uint64_t pte_index = extract_bits_from_va(virtual_address,12,20);
				(pt *)pt->pt_entries[pte_index]  = pa;
			}
		}
		else
		{
			uint64_t pd= page_alloc();
			memset(pd, 0, 4096);
			((pdp *)pdp)->pdp_entries[pdpt_index] = pd|3;
			uint64_t pde_index = extract_bits_from_va(virtual_address,21,29);
			uint64_t pt= page_alloc();
			memset(pt, 0, 4096);
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
		memset(pdp, 0, 4096);
		((pml4 *)pml4)->pml4_entries[pml4_index] = pdp|3;
		//now fill in the pdpt entry.
		uint64_t pdpt_index = extract_bits_from_va(virtual_address,30,38);
		kprintf("%d",pdpt_index);

		uint64_t pd= page_alloc();
		memset(pd, 0, 4096);

		((pdp *)pdp)->pdp_entries+pdpt_index = pd|3;
		uint64_t pde_index = extract_bits_from_va(virtual_address,21,29);
		kprintf("%d",pde_index);
		uint64_t pt= page_alloc();
		memset(pt, 0, 4096);
		((pd *)pd)->pd_entries[pde_index]= pt|3;


		uint64_t pte_index = extract_bits_from_va(virtual_address,12,20);
		kprintf("%d",pte_index);
		((pt *)pt)->pt_entries[pte_index]  = pa;
	}


	return;
}*/
//#include<stdio.h>
