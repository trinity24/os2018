#include <sys/defs.h>
#define PHYSFREE 0x20C000
#define PHYSBASE 0X200000

#define PAGESIZE 4096

struct Page {
	int status;
	uint64_t id;
	struct Page *next;

};
//struct Page *free_list;//.=NULL;
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

#define SIZE_TABLES 512

/*
struct pdp
{
	uint64_t pdp_entries[SIZE_TABLES];
};

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



uint64_t extract_bits_from_va(uint64_t virtual_address, int start);
void kernal_map(uint64_t kernbase, uint64_t physbase,uint64_t pml4);
void page_table_walk(uint64_t pa, uint64_t va,pml4 pml4_t);
void video_map(uint64_t , uint64_t ,pml4);
int create_list(uint64_t end,uint64_t physfree);
uint64_t page_alloc();

void set_page_free(int page);

int is_free(int page);

void print_free_list_head();
//void va_to_pa(uint64_t , uint64_t , pml4);
void map_va_to_pa(uint64_t , uint64_t ,pml4 );
uint64_t get_free_list_head();
uint64_t s_next_page(uint64_t);
void c_n_l( uint64_t , uint64_t );
