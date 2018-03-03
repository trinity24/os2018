#ifndef _PMAP_H
#define _PMAP_H
#include <sys/defs.h>
#define PHYSFREE 0x20C000
#define PHYSBASE 0X200000
#define KERNBASE 0xffffffff80000000
#define PAGESIZE 4096
#define SIZE_TABLES 512
void initialise_paging(uint64_t physbase,uint64_t physfree,uint64_t avail_mem_end);
void identical_map1(uint64_t pages, uint64_t base, pml4 pml4_t);
uint64_t extract_bits_from_va(uint64_t virtual_address, int start);
void kernal_map(uint64_t pages,uint64_t kernbase, uint64_t physbase,uint64_t physfree,uint64_t pml4);
void page_table_walk_k(uint64_t pa, uint64_t va,pml4 pml4_t,int flag);

void identical_mapping(uint64_t vm_va, uint64_t vmbase,pml4 pml4_t);
void video_map(uint64_t , uint64_t ,pml4);
void count_page(uint64_t address);

void identical_map(uint64_t end ,pml4 pml4_t);
void map_va_to_pa(uint64_t , uint64_t ,pml4 );
uint64_t get_paddr(uint64_t va,pml4 pml4_t);
void page_table_walk(uint64_t pa, uint64_t va,pml4 pml4_t);
uint64_t get_paddr_user(uint64_t va,pml4 pml4_t);
void c_n_l( uint64_t , uint64_t );
#endif
