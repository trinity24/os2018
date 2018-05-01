#ifndef MEM_H
#define MEM_H

#include <sys/defs.h>
#include <sys/task.h>
#define	KERNBASE 0xffffffff80000000

#define FREE 1
#define ALLOCATED 0

typedef uint64_t pml4e_t;
typedef uint64_t pdpe_t;
typedef uint64_t pte_t;
typedef uint64_t pde_t;

extern volatile pte_t vpt[];     // VA of "virtual page table"
extern volatile pde_t vpd[];     // VA of current page directory
extern volatile pde_t vpde[];     // VA of current page directory pointer
extern volatile pde_t vpml4e[];     // VA of current page map level 4

#define PAGESIZE 4096
#define PHYS_EXTENDED 0x100000
#define PAGES 8192
#define WORD_SIZE 64
#define MAX_TABLE_ENTRIES 512


struct Page {
    // Next page on the free list.
    // pp_ref is the count of pointers (usually in page table entries)
    // to this page, for pages allocated using page_alloc.
    // Pages allocated at boot time using pmap.c's
    // boot_alloc do not have valid reference count fields.
    uint32_t status;
    uint32_t id;
    uint32_t count;
    struct Page *next;
};

void print_initial_state(uint64_t physbase,uint64_t physfree);
void initialise_memory_management(uint32_t* modulep,uint64_t physfree,uint64_t avail_mem_end);
uint64_t get_avail_mem_end(uint32_t *modulep);
void mark_invalid_pages(uint64_t start, uint64_t end,uint64_t physfree);
extern struct Page *free_list;
void memcpy(void *p1,void * p2,uint64_t size);
void memset(uint64_t address, int data, int size);
void set_page_free(uint64_t address);
uint64_t get_free_list_head();
void print_free_list_head();
void clear_page(uint64_t pa);
void clear_page_k(uint64_t pa);
void clear_process_mem(pcb *task);
int is_free(int n);
uint64_t page_alloc_k();
uint64_t page_alloc();
int create_list( uint64_t end, uint64_t physfree);
uint64_t get_reference_count(uint64_t address);
void decrement_reference_count(uint64_t address);
#endif

