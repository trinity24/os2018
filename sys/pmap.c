//#include<stdio.h>
#include <sys/defs.h>
#include <sys/pmap.h>
#include <sys/kprintf1.h>
#include <sys/mem.h>
#include <stdlib.h>
static uint64_t fp;

void initialise_paging(uint64_t physbase,uint64_t physfree,uint64_t avail_mem_end)
{
	pml4 pml4_t = page_alloc();
	
        if( pml4_t != -1)
        {
                memset(pml4_t, 0, 4096);
                kernal_map(0, (uint64_t)KERNBASE+physbase,physbase,avail_mem_end,pml4_t);
                identical_mapping(KERNBASE, (uint64_t)physbase,pml4_t);
//              video_map(KERNBASE+(uint64_t)0xb8000, (uint64_t)0xb8000,pml4_t);
        }
	pml4 kcr3 = pml4_t;
	__asm__ volatile("mov %0, %%cr3"::"b"(kcr3));
	return;
}










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
uint64_t s_next_page(uint64_t physfree){
	fp = physfree;
	uint64_t temp;
	temp = fp;
	fp = fp + PAGESIZE;
	return temp;
}
uint64_t extract_bits_from_va(uint64_t virtual_address, int start)
{
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
        for(int i=0;i<pages;i++)
        {
                va= base+i*(PAGESIZE);
                pa= va;
                page_table_walk(pa,va,pml4_t);

        }
        return;
}


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
void kernal_map(uint64_t pages,uint64_t kernbase, uint64_t physbase,uint64_t end_avail ,pml4 pml4_t)
{

	int num_pages= (end_avail - physbase)/4096;
	uint64_t va,pa;
	kprintf("%d\n", (end_avail-physbase) /4096);
	kprintf("pml4 %p\n", pml4_t);
	for(int i=0;i<num_pages+pages;i++)
	{
		va=kernbase+i*(PAGESIZE);
		pa=physbase+i*(PAGESIZE);
		page_table_walk(pa,va,pml4_t);
		
	}

	free_list =(struct Page *) ((uint64_t)free_list+KERNBASE);
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
	int num_pages=pages;
	for(int i = 0; i < num_pages; i++){
		page_table_walk(base,base,pml4_t);
		base+= PAGESIZE;
	}
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

