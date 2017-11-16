#ifndef MEM_H
#define MEM_H

#include <sys/defs.h>

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
#define PHYSFREE 0x620B000

#define PHYS_EXTENDED 0x100000

#define PAGES 8192
#define WORD_SIZE 64
#define PAGE_SIZE 0x1000
#define MAX_TABLE_ENTRIES 512

 inline void* page2kva(struct Page *p)
{
    return KADDR(page2pa(pp));
}

typedef uint64_t phyaddr_t;

#define KADDR(pa)						\
    ({								    \
     uint64_t  __m_pa = (pa);		    \
     uint32_t __m_ppn = PPN(__m_pa);    \
     if (__m_ppn >= npages)				\
     print("KADDR called with invalid pa %08lx", __m_pa);   \
     (void*) ((uint64_t)(__m_pa + KERNBASE));				\
     })



 inline physaddr_t page2pa(struct Page *pp)
{
    return page2ppn(pp) << PGSHIFT;

}




struct Page {
    // Next page on the free list.
    // pp_ref is the count of pointers (usually in page table entries)
    // to this page, for pages allocated using page_alloc.
    // Pages allocated at boot time using pmap.c's
    // boot_alloc do not have valid reference count fields.

    uint16_t id;
  struct Page *next;
};


endif

