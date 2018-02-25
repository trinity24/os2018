#ifndef _PMAP_H
#define _PMAP_H
#include <sys/defs.h>
#define PHYSFREE 0x20C000
#define PHYSBASE 0X200000
#define KERNBASE 0xffffffff80000000
#define PAGESIZE 4096

struct Page {
	uint32_t status;
        uint32_t count;
	uint64_t id;
	struct Page *next;
};

struct Page *free_list ;
//static struct Page *v_free_list =NULL;
//uint64_t pages_allocated=0;

#define ALLOCATED 1
#define FREE 0

//uint64_t * get_pml4();
//extern uint64_t pml4;
//typedef uint64_t *;
//void memset(void *mem, int val, int len);
void memset(uint64_t mem, int val, int len);
//typedef struct pdp_table pdp;
//typedef struct pd_table pd;
//typedef struct pt_table pt;
void memcpy(void *p1,void * p2,uint64_t size);
#define SIZE_TABLES 512

/*
struct pdp
{
	uint64_t pdp_entries[SIZE_TABLES];
};
void memcpy(void *p1,void * p2,uint64_t size)

struct pml4
{
	uint64_t pml4_t_entries[SIZE_TABLES];
};

struct pd
{
	uint64_t pd_entries[SIZE_TABLES];
};
struct pt
{
	uint64_t pt_entries[SIZE_TABLES];
}; */


void identical_map1(uint64_t pages, uint64_t base, pml4 pml4_t);

uint64_t extract_bits_from_va(uint64_t virtual_address, int start);
void kernal_map(uint64_t pages,uint64_t kernbase, uint64_t physbase,uint64_t physfree,uint64_t pml4);
void page_table_walk_k(uint64_t pa, uint64_t va,pml4 pml4_t,int flag);

void identical_mapping(uint64_t vm_va, uint64_t vmbase,pml4 pml4_t);
void video_map(uint64_t , uint64_t ,pml4);
int create_list(uint64_t end,uint64_t physfree);
uint64_t page_alloc();
uint64_t page_alloc_k();
void set_page_free(uint64_t address);
void count_page(uint64_t address);

int is_free(int page);
void identical_map(uint64_t end ,pml4 pml4_t);
void print_free_list_head();
//void va_to_pa(uint64_t , uint64_t , pml4);
void map_va_to_pa(uint64_t , uint64_t ,pml4 );
uint64_t get_free_list_head();
uint64_t s_next_page(uint64_t);
uint64_t get_paddr(uint64_t va,pml4 pml4_t);
void page_table_walk(uint64_t pa, uint64_t va,pml4 pml4_t);
uint64_t get_paddr_user(uint64_t va,pml4 pml4_t);
void c_n_l( uint64_t , uint64_t );
#endif
