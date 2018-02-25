//#include<stdio.h>
#include <sys/defs.h>
#include <sys/pmap.h>
#include <sys/kprintf1.h>
#include <stdlib.h>

//#include <string.h>

struct Page *next_free_page;
static uint64_t fp;

uint64_t get_paddr_user(uint64_t va,pml4 pml4_t)
{
#define PA(va) (va - KERNBASE)
#define VA(pa) (pa + KERNBASE)
        uint64_t pml4_i = extract_bits_from_va(va, 39);
        uint64_t pdpe_i = extract_bits_from_va(va, 30);
        uint64_t pde_i = extract_bits_from_va(va, 21);
        uint64_t pte_i = extract_bits_from_va(va, 12);
        if( *((pml4 *)pml4_t + pml4_i) & 0x7)
        {
                pdp pdp_t = *((pml4 *)pml4_t + pml4_i) & 0xfffffffffffff000;
                if( *((pdp *)VA(pdp_t) + pdpe_i) & 0x7)
                {
                        pd pd_t = *((pdp *)VA(pdp_t) + pdpe_i) & 0xfffffffffffff000;
                        if(*((pd *)VA(pd_t) + pde_i) & 0x7)
                        {
                                pt pt_t = *((pd *)VA(pd_t) + pde_i) & 0xfffffffffffff000;
                                 return *((pt *)VA(pt_t) + pte_i) ;
                        }

                }
        }
        kprintf_k("We don't have this \n");
        return -1;
}
uint64_t get_paddr(uint64_t va,pml4 pml4_t)
{
#define PA(va) (va - KERNBASE)
#define VA(pa) (pa + KERNBASE)
	uint64_t pml4_i = extract_bits_from_va(va, 39);
        uint64_t pdpe_i = extract_bits_from_va(va, 30);
        uint64_t pde_i = extract_bits_from_va(va, 21);
        uint64_t pte_i = extract_bits_from_va(va, 12);
        if( *((pml4 *)pml4_t + pml4_i) & 0x3)
	{
                //pdp pdp_t = *((pml4 *)pml4_t + pml4_i) >> 12;
                pdp pdp_t = *((pml4 *)pml4_t + pml4_i) & 0xfffffffffffff000;
                if( *((pdp *)VA(pdp_t) + pdpe_i) & 0x3)
		{
                        //pd pd_t = *((pdp *)pdp_t + pdpe_i) >> 12;
                        pd pd_t = *((pdp *)VA(pdp_t) + pdpe_i) & 0xfffffffffffff000;
                        if(*((pd *)VA(pd_t) + pde_i) & 0x3)
			{
                                        //pt pt_t = *((pd *)pd_t + pde_i) >> 12;
                                pt pt_t = *((pd *)VA(pd_t) + pde_i) & 0xfffffffffffff000;
                                 return *((pt *)VA(pt_t) + pte_i) ;
                      	}	
	
		}
	}
	kprintf_k("We don't have this \n");
	return -1;
}
int create_list( uint64_t end, uint64_t physfree)
{
//	kprintf("Free list at the start of create_list is %p \n",free_list);
	extern	 struct Page *free_list;
	uint64_t  p=0,f=0;
	//	struct Page *prev=NULL;
	uint64_t paddr=0;
	//struct Page *page_free_head = NULL;
	struct Page *pages=(struct Page *)(physfree);
	uint64_t max= end/4096;
	struct Page *temp;
	uint64_t i = 0;
	uint64_t size_of_free_list = (end / 4096) * sizeof(struct Page);
	uint64_t number_of_pages_fl = (size_of_free_list) / PAGESIZE;
	for(i=0;i<max;i++)
	{
		p++;
		(pages+i*(sizeof(struct Page)))->id = paddr+i*PAGESIZE;
		if( paddr+i*PAGESIZE<(physfree + number_of_pages_fl * PAGESIZE) ){
			(pages+i*(sizeof(struct Page)))->status = ALLOCATED;
			(pages+i*(sizeof(struct Page)))->next = NULL;
		
		}
		else
		{
			if(f==0)
			{
				kprintf("%p is the physfree \n", physfree);	
				free_list= (pages+i*(sizeof(struct Page)));
				(pages+i*(sizeof(struct Page)))->status = FREE;
				(pages+i*(sizeof(struct Page)))->id =paddr+i*PAGESIZE;
				//kprintf("the address of free_list page is %p \n ",free_list->id);
				free_list->next=NULL;
				temp=free_list;
				f++;
				kprintf("%p is the first time free_list is updated and %p is address corresponding to it \n",free_list, free_list->id);
			}
			else
			{
				
				temp->next = (pages+i*(sizeof(struct Page)));//+KERNBASE;
				(pages+i*(sizeof(struct Page)))->id =paddr+i*PAGESIZE;
				(pages+i*(sizeof(struct Page)))->status = FREE;
				(pages+i*(sizeof(struct Page)))->next=NULL;
				temp=(pages+i*(sizeof(struct Page)));
				f++;
			}
		}
	}
	
	kprintf("%d is the number of free_pages\n",f);
	kprintf("The last page structure is stored at %p\n", (pages+(i - 1)*(sizeof(struct Page))));
	//kprintf("Free list at the end of create_list is %p  The address corresponding to the free_list %p \n",*free_list,free_list->id );
	kprintf("before returning from create list %p is the free_list->id \n",free_list->id);
	return (free_list->id);

}
void memset(uint64_t mem, int val, int len){
	char *p = NULL;
	p = (char *)mem;
	while(len > 0)
        {
	 	*p = val;
	 	p++;
	 	len--;
	 }
	return;
}


uint64_t page_alloc()
{
	//pages_allocated++;
	uint64_t addr;
	if(free_list!=NULL)
	{
		//pages_allocated++;	
		addr=free_list->id;
		//kprintf("%p is the address first time \n",addr);
		free_list->status=1;
                free_list->count=1;
		struct Page *temp= free_list->next;
		free_list->next=NULL;
		free_list=temp;
	//	addr;
//		kprintf("free_list now is- after allocating page %p",free_list);
		return addr;
	}
	else{
		kprintf("Pages not available, ran out of memory !!\n");
		return -1;
	}
	
}
uint64_t page_alloc_k()
{
        //pages_allocated++;
        uint64_t addr;
	
        if((struct Page *) ((uint64_t)(free_list)+KERNBASE) !=NULL)
        {
                //pages_allocated++;    
                addr=(uint64_t)(((struct Page *)((uint64_t)(free_list)+KERNBASE))->id)+KERNBASE;
                //kprintf("%p is the address first time \n",addr);
                ((struct Page *)((uint64_t)(free_list)+KERNBASE))->status=1;
		((struct Page *)((uint64_t)(free_list)+KERNBASE))->count=1;
                struct Page *temp= (((struct Page *)((uint64_t)(free_list)+KERNBASE))->next);
                ((struct Page *)((uint64_t)(free_list)+KERNBASE))->next=NULL;
		

		// HERE MY FREE_LIST SHOULD BE UPDATED... HOW?

		//free_list = ((struct Page *)((uint64_t)(free_list)+KERNBASE))->next;       
		//      kprintf("free_list now is- after allocating page %p",free_list);
        	struct Page *x;
		x= (struct Page *) ((uint64_t)(free_list)+KERNBASE); 
		x = temp;
		free_list = x;
//	   	(struct Page *) ((uint64_t)(free_list)+KERNBASE) = temp;
		return addr;
        }
        else{
                kprintf("Pages not available, ran out of memory !!\n");
                return -1;
        }

}






/*

uint64_t page_alloc_k()
{
	//kprintf("%p is the v_free_list \n",v_free_list);
        uint64_t addr;
        if(free_list!=NULL)
        {
                addr=free_list->id;
                free_list->status=1;
                struct Page *temp= v_free_list->next;
                free_list->next=NULL;
                free_list=temp;
//              kprintf("free_list now is- after allocating page %p",free_list);
                return addr;
        }
        else{
                kprintf("Pages not available, ran out of memory !!\n");
                return -1;
        }

}*/

uint64_t s_next_page(uint64_t physfree){
	fp = physfree;
	uint64_t temp;
	temp = fp;
	fp = fp + PAGESIZE;
	return temp;
}

uint64_t get_free_list_head(){
	return (uint64_t)free_list;
}

void print_free_list_head(){
	kprintf("Free list head is: %p\n", free_list);
	next_free_page = free_list->next;
	kprintf("Next free page: %p\n", next_free_page);
}


int is_free(int n)
{
	if(((struct Page *)(PHYSFREE +(uint64_t) n))->status==1)
		return 0;
	else
		return 1;
}

void count_page(uint64_t address)
{
        uint64_t pa= address-KERNBASE;
        int n=(pa/PAGESIZE); 
        struct Page* pages = (struct Page*) PHYSFREE;
	 if (pages[n].status != 1 || pages[n].count == 0)
        {       
                // TODO : remove this
                kprintf_k("Count page : Impossible case\n");
                while(1);
        }
	pages[n].count++;
	return;
		
}

void set_page_free(uint64_t address)
{
	
	uint64_t pa= address-KERNBASE;
	int n=(pa/PAGESIZE);
	struct Page* pages = (struct Page*) PHYSFREE;

        if (pages[n].status != 1 || pages[n].count == 0)
	{
		// TODO : remove this
		kprintf_k("set_page_free : Impossible case\n");
		while(1);
	}
		
//	if((((struct Page *)(PHYSFREE +(uint64_t)n*sizeof(struct Page))))->status==1)
	pages[n].count--;
	if (pages[n].count == 0)
	{
		pages[n].status = 0;
		
		//((struct Page *)(PHYSFREE + (uint64_t)n))->status=0;
		//struct Page *temp =((struct Page *)(PHYSFREE + (uint64_t)(n)))->next = free_list;
		pages[n].next = free_list;
		free_list = pages + n;
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

void identical_map1(uint64_t pages,uint64_t base ,pml4 pml4_t)

{
        uint64_t va,pa;
        //kprintf("%d\n", (physfree-physbase) /4096);
       // kprintf("pml4 %p\n", pml4_t);
        for(int i=0;i<pages;i++)
        {
                va= base+i*(PAGESIZE);
                pa= va;
                page_table_walk(pa,va,pml4_t);

        }
        //v_free_list = free_list+kernbase;
        //kprintf("%p is the v_free_list \n",v_free_list);
        return;
}
/*
void identical_map(uint64_t pages,uint64_t kernbase, uint64_t physbase,uint64_t physfree ,pml4 pml4_t)
{
	uint64_t num_pages= (kernbase+(physfree-physbase))/4096;
	uint64_t va,pa;
	uint64_t repeat_for = 0xffffffff;
	for(uint64_t i=0;i<(num_pages+pages);i++)
        {
                va=i*(PAGESIZE);
                pa=(i*(PAGESIZE))%(repeat_for);
                page_table_walk(pa,va,pml4_t);
        }
	return;

}
*/
void identical_map(uint64_t end_availble_mem ,pml4 pml4_t)
{
        
        int num_pages= (end_availble_mem )/4096;
        uint64_t va,pa; 
        kprintf("%d\n", (end_availble_mem) /4096);
        kprintf("pml4 %p\n", pml4_t);
        for(int i=0;i<num_pages;i++)
        {       
                va= KERNBASE+i*(PAGESIZE);
                pa=i*(PAGESIZE);
                page_table_walk(pa,va,pml4_t);
         
        }
        
        free_list =(struct Page *) ((uint64_t)free_list+KERNBASE);
        return;
}
void kernal_map(uint64_t pages,uint64_t kernbase, uint64_t physbase,uint64_t physfree ,pml4 pml4_t)
{

	int num_pages= (physfree - physbase)/4096;
	uint64_t va,pa;
	kprintf("%d\n", (physfree-physbase) /4096);
	kprintf("pml4 %p\n", pml4_t);
	for(int i=0;i<num_pages+pages;i++)
	{
		va=kernbase+i*(PAGESIZE);
		pa=physbase+i*(PAGESIZE);
		page_table_walk(pa,va,pml4_t);
		
	}

	free_list =(struct Page *) ((uint64_t)free_list+KERNBASE);
//	kprintf("%p is the v_free_list \n",v_free_list);
	return;
}
void video_map(uint64_t vm_va, uint64_t vmbase,pml4 pml4_t)
{
        page_table_walk(vmbase,vm_va,pml4_t);
	page_table_walk(vmbase+1*(PAGESIZE),vm_va+1*(PAGESIZE),pml4_t);
	page_table_walk(vmbase+2*(PAGESIZE),vm_va+2*(PAGESIZE),pml4_t);
  return;
}
void identical_mapping(uint64_t vm_va, uint64_t vmbase,pml4 pml4_t)
{
	uint64_t num_pages=vmbase/PAGESIZE;
	
	for(uint64_t i=0;i<num_pages;i++)
		page_table_walk(i*(PAGESIZE),vm_va+i*(PAGESIZE),pml4_t);
	return;
}
void map_va_to_pa(uint64_t pages, uint64_t base, pml4 pml4_t)
{
	//kprintf("Trying to map %p to %p", va, base);
	//kprintf("Trying to map %p, num pages = %d\n", base, (pml4_t - base)/4096);
	int num_pages=pages;
	//num_pages = (pml4_t - base)/PAGESIZE;
	for(int i = 0; i < num_pages; i++){
		//kprintf("Trying to map %p to %p", va, base);
		page_table_walk(base,base,pml4_t);
		//va=va+PAGESIZE;
		base+= PAGESIZE;
	}
   
  //page_table_walk(base,va,pml4_t);
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
				//kprintf("page allocated is %p\n",s_next_page()
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



void page_table_walk_k(uint64_t pa, uint64_t va,pml4 pml4_t,int flag)
{
#define PA(va) (va - KERNBASE)
#define VA(pa) (pa + KERNBASE)
        uint64_t pml4_i = extract_bits_from_va(va, 39);
        uint64_t pdpe_i = extract_bits_from_va(va, 30);
        uint64_t pde_i = extract_bits_from_va(va, 21);
        uint64_t pte_i = extract_bits_from_va(va, 12);
	
        if( *((pml4 *)pml4_t + pml4_i) & flag){
                //pdp pdp_t = *((pml4 *)pml4_t + pml4_i) >> 12;
                pdp pdp_t = *((pml4 *)pml4_t + pml4_i) & 0xfffffffffffff000;
                if( *((pdp *)VA(pdp_t) + pdpe_i) & flag){
                        //pd pd_t = *((pdp *)pdp_t + pdpe_i) >> 12;
                        pd pd_t = *((pdp *)VA(pdp_t) + pdpe_i) & 0xfffffffffffff000;
                        if(*((pd *)VA(pd_t) + pde_i) & flag){
                                        //pt pt_t = *((pd *)pd_t + pde_i) >> 12;
                                        pt pt_t = *((pd *)VA(pd_t) + pde_i) & 0xfffffffffffff000;
                                        *((pt *)VA(pt_t) + pte_i) = pa | flag;
                                        //kprintf("%p %p %p %p %p\n", pml4_t, *((pml4 *)pml4_t + pml4_i), *((pdp *)pdp_t + pdpe_i), *((pd *)pd_t + pde_i), *((pt *)pt_t + pte_i));
                        }else{
                                //create pt
                                //kprintf("page allocated is %p\n",s_next_page()
                                pt pt_t = page_alloc_k();
                                if(pt_t != -1){
                                        memset(pt_t, 0, 4096);
                                        *((pd *)VA(pd_t) + pde_i) = PA(pt_t) | flag;

                                        *((pt *)pt_t + pte_i) = pa | flag;
                                        //kprintf("%p %p %p %p %p\n", pml4_t, *((pml4 *)pml4_t + pml4_i), *((pdp *)pdp_t + pdpe_i), *((pd *)pd_t + pde_i), *((pt *)pt_t + pte_i));
                                }else{
                                        kprintf_k("Ran out of free memory\n");
                                }
                        }

                }else{
                        //create pd
                        pd pd_t = page_alloc_k();
                        if(pd_t != -1){
                                memset(pd_t, 0, 4096);
                                *((pdp *)pdp_t + pdpe_i) = PA(pd_t)|flag;

                                pt pt_t = page_alloc_k();
                                if(pt_t != -1){
                                        memset(pt_t, 0, 4096);
                                        *((pd *)pd_t + pde_i) = PA(pt_t) |flag;

                                        *((pt *)pt_t + pte_i) = pa | flag;
                                }else{
                                        kprintf_k("Ran out of free memory\n");
                                }
                        }
                        else{
                                kprintf_k("Ran out of free memory\n");
                        }

                }
        }else{
                pdp pdp_t = page_alloc_k();
               if(pdp_t != -1){
                        memset(pdp_t, 0, 4096);
                        *((pml4 *)pml4_t + pml4_i) = PA(pdp_t)|flag;

                        pd pd_t = page_alloc_k();
                        if(pd_t != -1){
                                memset(pd_t, 0, 4096);
                                *((pdp *)pdp_t + pdpe_i) = PA(pd_t) | flag;

                                pt pt_t = page_alloc_k();
                                if(pt_t != -1){
                                        memset(pt_t, 0, 4096);
                                        *((pd *)pd_t + pde_i) = PA(pt_t)|flag;

                                        *((pt *)pt_t + pte_i) = pa |flag;
                                }else{
                                        kprintf_k("Ran out of free memory\n");
                                }
                        }else{
                                        kprintf_k("Ran out of free memory\n");
                        }
                }else{
                        kprintf_k("Ran out of free memory\n");
                }
        }
		

	//TODO:TLB FLUSH- relaoding the cr3
        //pml4 cur_pml4;
 	 __asm__ volatile("movq %%cr3, %%rax; movq %%rax, %%cr3":::"%rax");
 	 //__asm__ volatile("movq %%cr3, %0": "=b"((uint64_t)cur_pml4):);
	//uint64_t my_pa=get_paddr_user(va,pml4_t);
	//kprintf_k("%x is the my_pa",my_pa);
        return;

}

