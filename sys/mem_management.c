#include <sys/mem.h>
#include <sys/kprintf1.h>
#include <sys/pmap.h>
uint64_t physfree_t;
struct Page *next_free_page;
void memcpy(void *p1,void * p2,uint64_t size)
{
        char *dst = (char *)p1;
        char *src= (char*)p2;
        while(size--)
        {
                *dst  =*src ;
                 src++;
                 dst++;

        }
        return;
}
struct Page *free_list =NULL;

int create_list( uint64_t end, uint64_t physfree)
{

        uint64_t  p=0,f=0;
        uint64_t paddr=0;
        //freepages_list starts at physfree 
        struct Page *pages=(struct Page *)(physfree);
        uint64_t max_page= (end-0)/PAGESIZE;
        struct Page *temp;
        
        uint64_t size_of_free_list = max_page * sizeof(struct Page);
        uint64_t number_of_pages_fl = (size_of_free_list) / PAGESIZE;
        
        //creating pages for entire memory 
        uint64_t i;
        for(i=0;i<max_page;i++)
        {       
                p++;
                pages[i].id = paddr+i*PAGESIZE;
                // mark all pages from 
                if( paddr+i*PAGESIZE < (physfree + number_of_pages_fl * PAGESIZE) ){
                        pages[i].status = ALLOCATED;
                        pages[i].next = NULL;
                
                }
                else
                {       
                        if(f==0)
                        {       
                                kprintf("%p is the physfree \n", physfree);
                                free_list= &(pages[i]);
                                pages[i].status = FREE;
                                pages[i].count=1;
                                pages[i].id =paddr+i*PAGESIZE;
                                free_list->next=NULL;
                                temp=free_list;
                                f++;
                        }
                        else
                        {
                                
                                temp->next = &(pages[i]);//+KERNBASE;
                                pages[i].id =paddr+i*PAGESIZE;
                                pages[i].count=1; 
                                pages[i].status = FREE;
                                pages[i].next=NULL;
                                temp=&(pages[i]);
                                f++;
                        }
                }
        }
        
        return (uint64_t)free_list;

}


uint64_t page_alloc()
{
        uint64_t addr;
        if(free_list!=NULL)
        {
                addr=free_list->id;
                free_list->status=1;
                free_list->count=1;
                struct Page *temp= free_list->next;
                free_list->next=NULL;
                free_list=temp;
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
                addr=(uint64_t)(((struct Page *)((uint64_t)(free_list)+KERNBASE))->id)+KERNBASE;
                ((struct Page *)((uint64_t)(free_list)+KERNBASE))->status=1;
                ((struct Page *)((uint64_t)(free_list)+KERNBASE))->count=1;
                struct Page *temp= (((struct Page *)((uint64_t)(free_list)+KERNBASE))->next);
                ((struct Page *)((uint64_t)(free_list)+KERNBASE))->next=NULL;
                // HERE MY FREE_LIST SHOULD BE UPDATED... HOW?
                struct Page *x;
                x= (struct Page *) ((uint64_t)(free_list)+KERNBASE);
                x = temp;
                free_list = x;
                return addr;
        }
        else
	{
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

void print_free_list_head()
{
        kprintf("Free list head is: %p\n", free_list);
        next_free_page = free_list->next;
        kprintf("Next free page: %p\n", next_free_page);
}

uint64_t get_free_list_head()
{
        return (uint64_t)free_list;
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

        pages[n].count--;
        if (pages[n].count == 0)
        {
                pages[n].status = 0;
                pages[n].next = free_list;
                free_list = pages + n;
        }
        return;
}

void memset(uint64_t address, int data, int size)
{
	char *temp=NULL;
	temp =(char *)address;
	while(size--)
		*temp++=data;
	return;
}

void mark_invalid_pages(uint64_t start, uint64_t end,uint64_t physfree)
{
	struct Page *temp= (struct Page*)(physfree);	
	for(int i=0;i<end-start;i++)
	{
		temp[i].status=-1;
		temp[i].next=0;
		
	}
	return;

}

uint64_t get_avail_mem_end(uint32_t *modulep)
{
         struct smap_t {
                uint64_t base, length;
                uint32_t type;
        }__attribute__((packed)) *smap;
        uint64_t mem_base=0, mem_len=0;

        while(modulep[0] != 0x9001) modulep += modulep[1]+2;

        for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
                if (smap->type == 1 /* memory */ && smap->length != 0) {
                        kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
                        mem_base=smap->base;mem_len=smap->length;
                }

        }
        return (mem_base+mem_len);


}
void initialise_memory_management(uint32_t *modulep,uint64_t physfree,uint64_t avail_mem_end)
{
	physfree_t = physfree;

        struct smap_t {
                uint64_t base, length;
                uint32_t type;
        }__attribute__((packed)) *smap;
        int first_chunk=0;
        uint64_t mem_base, mem_len,prev_valid=0;
        while(modulep[0] != 0x9001) modulep += modulep[1]+2;
        
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) 
	{
                
		if (smap->type == 1 /* memory */ && smap->length != 0) 
		{
                        mem_base=smap->base;mem_len=smap->length;
                        if(first_chunk==0)
                        {
                                uint64_t avail_mem_end = get_avail_mem_end(modulep);
                        	prev_valid=mem_len+mem_base; 
			        //create free_list and mark all the mem locations of current ranges 
                                create_list(avail_mem_end,(physfree));
                        }
			else if(prev_valid!=-1)
			{
				//now mark invalid pages between the availablemem chunks
				mark_invalid_pages(prev_valid,mem_base,physfree);
				prev_valid=mem_base+mem_len;		
			}
                }
        }
	
	return;	

}
void print_initial_state(uint64_t physbase, uint64_t physfree)
{
	kprintf("Initial Values of Physbase : %x and Physfree : %x \n",physbase,physfree);
	return;
}

void count_page(uint64_t address)
{
        uint64_t pa= address-KERNBASE;
        int n=(pa/PAGESIZE);
        struct Page* pages = (struct Page*) (KERNBASE + physfree_t);

        if (pages[n].status != 1 || pages[n].count == 0)
        {
                // TODO : remove this
                kprintf_k("Count page : Impossible case\n");
                while(1);
        }
        pages[n].count++;
        return;

}

