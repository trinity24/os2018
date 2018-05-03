//#include <sys/elf64.h>

#include <sys/process.h>
#include <sys/tarfs.h>
#include <sys/defs.h>
#include <sys/string.h>
#include <sys/task.h>
#include <sys/pmap.h>
#include <sys/elf64.h>
#include <sys/kprintf1.h>
#include <sys/gdt.h>
#include <sys/mem.h>
#include <sys/tarfs.h>

pcb *t1,*t2,*t3;
pcb all_tasks[PROCESS_COUNT];
uint64_t pid=0;
vm_struct *vm= NULL;
#define URP(addr) (addr&(0xfffffffffffffff5))
#define CLR(addr) (addr&(0xfffffffffffff000))
void exit_syscall(int status);
pcb* new_task();

extern char read_buff[100];
extern int read_it;
extern int read_linecount;
void initialise_files(pcb *task);
void check_path(char *path,char *new_path);
void get_full_path(char *path,char *new_path);
int is_directory(char *path);
int is_directory(char *path);
int get_file_ptr(char *path,char **fp);
int get_file_des(pcb *task);
void schedule()
{       
	pcb *next_task=NULL;
	int flag=0;
	for(int i=(curr_task->pid)+1;i<PROCESS_COUNT;i++)
	{
		if(all_tasks[i].state==RUNNING_P)
		{
			flag=1;
			next_task= &all_tasks[i];
			break;
		}
	}

	if(!flag)
	{
		flag=0;
		for(int i=0;i<=(curr_task->pid);i++)
		{
			 if(all_tasks[i].state==RUNNING_P)
                	{       
				flag=1;
                        	next_task= &all_tasks[i];
				break;
                	}
		}
	}
	
	if(flag)
	{
		//kprintf_k("Scheduling %d\n", next_task->pid);
		pcb *temp=curr_task;
		curr_task=next_task;
                set_tss_rsp( &curr_task->kstack[510]);
		if (next_task->kstack[511] == 92736)
		{
			//next_task->rsp = (uint64_t) &(next_task->kstack[496-7-15]);
			next_task->kstack[496-7-15+8] = 0;
			next_task->kstack[496-7-15+7] = 0;
			next_task->kstack[496-7-15+9] = 0;
			//TODO: put rax=0 in the required offset on kstack;
			next_task->kstack[511] = 0;
		} 
        	context_switch_routine(temp, next_task);
	}
	else
	{
		kprintf_k("schedule - impossible condition\n");
	}
	return;
}
void idle()
{
	while(1)
	{
		__asm__ volatile("sti; hlt; cli;");
		schedule();
		
	}
}
uint64_t get_cr3()
{
	uint64_t cr3;
         __asm__ volatile("movq %%cr3,%0":"=b"(cr3):);
	return cr3;
}
uint64_t get_cr2()
{	
	uint64_t cr2;
         __asm__ volatile("movq %%cr2,%0":"=b"(cr2):);
	return cr2;
}
void tlb_flush()
{
	__asm__ volatile("movq %%cr3, %%rax; movq %%rax, %%cr3":::"%rax");
}

void slacked_forever()
{
//	int status;	
	while(1)
	{
		//wait_syscall(&status);
	}
}

void initialise_tasks()
{
	for(int i=0;i<PROCESS_COUNT;i++)
	{ 	
		all_tasks[i].pid= i;
		initialise_files(&all_tasks[i]);
	}
	//initialise the head process
	//slacked_forever();
	pcb *task1= new_task();
	
//	task1->rsp = task1->kstack[510];
//	run_queue_add(task1);	
//	task1->rip =(uint64_t) &slacked_forever;
//	task1= new_task();
        task1->rsp = (uint64_t)&task1->kstack[510-15];
        run_queue_add(task1);
        task1->kstack[510] = (uint64_t) idle;
	uint64_t kernel_cr3 = get_cr3();
	task1->pml4_t = kernel_cr3;
	
	return;
}

pcb* new_task()
{
	for(int i=0;i<PROCESS_COUNT;i++)
	{
		if(all_tasks[i].state==UNALLOCATED_P)
		{
			memset((uint64_t)&all_tasks[i], 0, sizeof(pcb));
			all_tasks[i].pid = i;
			all_tasks[i].state =ALLOCATED_P;
			char path[2]="/";
			strcpy(all_tasks[i].cwd,path);
			return &all_tasks[i]; 
		}
	}
	kprintf_k("No processes can be accomodated\n");
	return NULL;
}

void run_queue_add(pcb *task)
{
	task->state=RUNNING_P;
	return;
}

void wait_queue_add(pcb *task)
{
	
	task->state=WAITING_P;
	return;
}
void read_suspend_add(pcb *task)
{
	task->state = READ_SUSPEND_P;
	return;
}
void read_suspend_remove()
{
	//kprintf_k("Reading suspended\n");
	for(int i=0;i<PROCESS_COUNT;i++)
	{
		if(all_tasks[i].state==READ_SUSPEND_P)
		{	
			run_queue_add(&all_tasks[i]);
			break;
		}
	}
	return;	
}

void clear_pcb(pcb* task)
{
	task->state= UNALLOCATED_P;
	
}	
uint64_t wait_syscall(int *status)
{

	//check for the zombies ... clear the pcbs if there are zombies
	for(int i=0;i<PROCESS_COUNT;i++)
	{
		if(all_tasks[i].state==ZOMBIE_P && all_tasks[i].parent_pid==curr_task->pid)
                {
			*status= all_tasks[i].exit_status;
                        //clear this child's pcb
                        clear_pcb(&all_tasks[i]);
                        return i;
                }
	}
	//adding current task to wait queue. 
	wait_queue_add(&all_tasks[curr_task->pid]);
	schedule();
	for(int i=0;i<PROCESS_COUNT;i++)
       	{       
                if(all_tasks[i].state==ZOMBIE_P && all_tasks[i].parent_pid==curr_task->pid)
               	{       
			//clear this child's pcb
                        clear_pcb(&all_tasks[i]);
                       	return i;
                }
        	
	}
	return -1;	
}
uint64_t waitpid_syscall(int pid,int *status)
{
/*
The value of pid can be:

< -1 : meaning wait for any child process whose process group ID is equal to the absolute value of pid.

-1   : meaning wait for any child process.

0    : meaning wait for any child process whose process group ID is equal to that of the calling process.

> 0  : meaning wait for the child whose process ID is equal to the value of pid.*/	
	if( pid>PROCESS_COUNT-1)
        {
                kprintf_k("Invalid Process id\n");
                return -1;
        }
        else if(all_tasks[pid].state==UNALLOCATED_P)
        {
                kprintf_k("This process doesn't exist \n");
                return -1;
        }
        else if(pid==-1)
        {
                return wait_syscall(status);
        }
        else if(pid <-1)
        {
                //take pid of the absolute process 
                pid*=-1;
        }
	while(1)	
        {
  		if(all_tasks[pid].state==ZOMBIE_P && all_tasks[pid].parent_pid==curr_task->pid)
        	{
                	//clear this child's pcb
			*status=all_tasks[pid].exit_status;
			clear_pcb(&all_tasks[pid]);
               		return pid;
        	}
        	else
		{
			wait_queue_add(&all_tasks[curr_task->pid]);
        		schedule();
		}
	}        
	return -1;
}

void ps_syscall()
{
	
	for(int i=0;i<PROCESS_COUNT;i++)
	{
		if(all_tasks[i].state!=UNALLOCATED_P)
		{
			kprintf_k("Process ID: %d \n",all_tasks[i].pid);
		}
	}
	return;
}

uint64_t kill_syscall(int process_id,int signal)
{
	if(process_id<0 || process_id>PROCESS_COUNT-1)
	{
		kprintf_k("Invalid Process id\n");
		return -1;
	}
	else if(all_tasks[process_id].state==UNALLOCATED_P)
	{
		kprintf_k("This process doesn't exist \n");
		return -1;
	}
	exit_syscall(1);
	return 1;
}

void adopt_orphan(int parent_id)
{
	for(int i=0;i<PROCESS_COUNT;i++)
	{
		if(all_tasks[i].parent_pid==parent_id)
			all_tasks[i].parent_pid=HEAD_PROCESS;
	}
	return;
}
void exit_syscall(int status)
{
	//change children parent if there are children
	adopt_orphan(curr_task->pid);
	//now, see if its parent is waiting and release it.
	run_queue_add(&all_tasks[curr_task->parent_pid]);
	//now prepare to terminate the child 
	clear_process_mem(&all_tasks[curr_task->pid]);
        all_tasks[curr_task->pid].state=ZOMBIE_P;
	all_tasks[curr_task->pid].exit_status=status;
	schedule();
	return;
}

char* getcwd_syscall(char *cwd,int size)
{
       int len = strlen(curr_task->cwd); 
        for(int i=0;i<len;i++)
        {
               cwd[i]= curr_task->cwd[i];
        
        }
	cwd[len] = '\0';
	return cwd;
}


int chdir_syscall(char *path)
{
	if(path==NULL)
		return -1;
	else if(path[0]=='/'&&path[1]=='\0')
	{
		
		strcpy(curr_task->cwd,path);
		return 0;
	}
	char new_path[256];
	check_path(path, new_path);
	if(is_directory(new_path)!=0)
	{
		
		strcpy(curr_task->cwd, new_path);	
		return 0;
	}
	return -1;
}

uint64_t get_pid()
{
	//kprintf_k("ITS IN GETPID\n");
	return curr_task->pid;
}
uint64_t get_ppid()
{
	return curr_task->parent_pid;
}

void function1()
{       
        kprintf_k("HEY I AM TASK1 -- In function task-1 now \n");
        schedule(t1,t2);
        kprintf("Done with it\n");
        return;
}
void function2()
{
        
        kprintf_k("HEY I AM TASK2 -- In function task-2 now \n");
        schedule(t2,t3);
        kprintf_k("HEY I AM TASK2  - Time 2");
        schedule(t2,t3);
        kprintf_k("HEY I AM TASK2  - Time 3");
        schedule(t2,t3);
        return;
}
void function3()
{       
        kprintf_k("HEY I AM TASK3 -- Came from TASK2 now \n");
        schedule(t3,t2);
        kprintf_k("HEY I AM TASK3 -- Tim 2\n");
        schedule(t3,t2);
        kprintf_k("HEY I AM TASK3 -- Tim 3\n");
        while(1);
        schedule(t3,t2);
        return;

}
int oct2bin( char *str, int size)
{
    int n = 0;
    char *c = str;
    while (size > 0) {
        n *= 8;
        n += *c - '0';
        c++;
	size--;
    }
    return n;
}
void stub_func()
{
	kprintf_k("here i am hello !\n");
	while(1);
}
void load_userprogram_content(vm_struct *list, uint64_t user_cr3,uint64_t kernel_cr3)
{
	vm_struct *vm = list;
	while(vm)
	{
		if(vm->type==0)
		{
			//creating mapping
			uint64_t start= vm->address;
			uint64_t end =vm->address +vm->size;
			uint64_t paddr= vm->fp;
			uint64_t num_pages =(vm->size/PAGESIZE);
			uint64_t extra_page= vm->size% PAGESIZE;
			if(extra_page)
				num_pages+=1;
			for(int i=0;i<num_pages;i++)
			{
				uint64_t page= page_alloc_k();	
				//copy - content; 
				if(num_pages)
				{
					//copying the first page.
					//we have to copy only half of the page.
							
					if(i==0 &&num_pages==1) //only one page, first page and less than 1 Page.
					{
						//copy first-half 
						uint64_t lower_chunk =start- (start&PAGEALIGN);
						//copy 0 till start
						if(lower_chunk)
							memset(page,0,lower_chunk);
						uint64_t total_content= end-start;
		
						//copy content from start to end
						//changing the cr3 to allowing copying
						 __asm__ volatile("movq %0, %%cr3"::"b"(kernel_cr3));
        					//tlb flush
						tlb_flush();
						memcpy((void *)(page+lower_chunk),(void*)(paddr),total_content);
						//switching back from kernel to user
						__asm__ volatile("movq %0, %%cr3"::"b"(user_cr3));
                                                //tlb flush
                                                __asm__ volatile("movq %%cr3, %%rax; movq %%rax, %%cr3":::"%rax");  
							
						uint64_t upper_chunk = PAGESIZE- total_content - lower_chunk;
						//copy 0 from end till the end of the page
						memset(page+end, 0,upper_chunk );
						//adding page to user pagetable
						page_table_walk_k(page-KERNBASE,start&PAGEALIGN,(pml4)(user_cr3+KERNBASE),0x7);  
					}
					//last page
					else if(i==num_pages-1)
					{
						//copy lower chunk with content;
						uint64_t page_begin = (start&PAGEALIGN)+i*PAGESIZE;
						uint64_t page_end = end;
						uint64_t lower_chunk = page_end- page_begin; 
						uint64_t copy_from = paddr;
						//((paddr)&PAGEALIGN)+ i*PAGESIZE; 
						__asm__ volatile("movq %0, %%cr3"::"b"(kernel_cr3));
                                                __asm__ volatile("movq %%cr3, %%rax; movq %%rax, %%cr3":::"%rax");  
						memcpy((void*)page,(void*)copy_from,lower_chunk);
						__asm__ volatile("movq %0, %%cr3"::"b"(user_cr3));
                                                //tlb flush
                                                __asm__ volatile("movq %%cr3, %%rax; movq %%rax, %%cr3":::"%rax");
						//copy 0 in rest of the page
						memset(page+lower_chunk,0,PAGESIZE-lower_chunk);	
						page_table_walk_k(page-KERNBASE, page_begin, (pml4)(user_cr3+KERNBASE),0x7);
					}
					//any page in between
					else
					{
						uint64_t lower_chunk =0;
						if(i==0)
						{
							//see if the page starts from half;
							lower_chunk = start-(start&PAGEALIGN);
							if(lower_chunk)	
								memset(page,0,lower_chunk);
							__asm__ volatile("movq %0, %%cr3"::"b"(kernel_cr3));
                                               		 //tlb flush
							uint64_t total_content= PAGESIZE-lower_chunk;
                                               		tlb_flush();
                                                	memcpy((void *)(page+lower_chunk),(void*)(paddr),total_content);
                                                	//switching back from kernel to user
                                                	__asm__ volatile("movq %0, %%cr3"::"b"(user_cr3));
                                                	//tlb flush
                                               	 	__asm__ volatile("movq %%cr3, %%rax; movq %%rax, %%cr3":::"%rax");		
							 page_table_walk_k(page-KERNBASE,start&PAGEALIGN,(pml4)(user_cr3+KERNBASE),0x7);
							paddr += total_content;
						}
						else
						{
							uint64_t page_begin = (start&PAGEALIGN)+i*PAGESIZE;
							//	uint64_t page_end = page_begin +PAGESIZE;
							uint64_t copy_from = paddr;
							//((paddr)&PAGEALIGN)+ i*PAGESIZE;
							__asm__ volatile("movq %0, %%cr3"::"b"(kernel_cr3));
                                                	//tlb flush
							memcpy((void *)(page+lower_chunk), (void *)copy_from,PAGESIZE);
							__asm__ volatile("movq %0, %%cr3"::"b"(user_cr3));
                                               		 //tlb flush
                                               		__asm__ volatile("movq %%cr3, %%rax; movq %%rax, %%cr3":::"%rax");
							page_table_walk_k(page-KERNBASE, page_begin, (pml4)(user_cr3+KERNBASE),0x7);
							paddr += PAGESIZE;
						}	
					
					}
				}
			}
		}
		vm=vm->next;
	}
	return;
}
/*	
void allocate_stack()
{
	uint64_t page=page_alloc_k();
	memset(page,0,PAGESIZE);
	page_table_walk_k(page-KERNBASE,STACKTOP-4096,7);
	return;
}
*/
void switch_to_user_mode(int create_stack, uint64_t args_page, uint64_t stack_top)
{
        //changing address space. Also, needs to be
	__asm__ volatile("movq %0,%%rsp;" : "=b"(curr_task->rsp));
        set_tss_rsp( &curr_task->kstack[510]);
	pml4 *user_pml4 = (pml4 *)page_alloc_k();
	uint64_t kernel_cr3;
	 __asm__ volatile("movq %%cr3,%0":"=b"(kernel_cr3):);

	uint64_t user_stack = (create_stack) ? page_alloc_k() : args_page;
	page_table_walk_k(user_stack-KERNBASE,STACKTOP-4096,(pml4)user_pml4,0x7);	
        curr_task->ustack= (create_stack) ? (STACKTOP-16) : stack_top;
        //copy page tables of kernel to user pagetables.
	*(pml4 *)((uint64_t)user_pml4 + 511*8) = *((pml4 *)(kernel_cr3+KERNBASE+511*8));
	//now load the cr3 with the user_pml4
	curr_task->pml4_t =((uint64_t)(user_pml4))-KERNBASE;
	__asm__ volatile("movq %0, %%cr3"::"b"((uint64_t)user_pml4-KERNBASE));
	//tlb flush
	__asm__ volatile("movq %%cr3, %%rax; movq %%rax, %%cr3":::"%rax");
 	//now load the cr3 with the user_pml4 	
	uint64_t user_cr3= (uint64_t)user_pml4-KERNBASE;
	load_userprogram_content(curr_task->vm_head,user_cr3,kernel_cr3 );
	
//	__asm__ volatile("mov %0, %%cr3"::"b"((uint64_t)user_pml4-KERNBASE));
//	uint64_t user_stack = page_alloc_k();
	//kprintf_k("Loading the User .. \n");	
        __asm__ volatile ("cli;"
			  "pushq $0x23;" //push the data segmnet
                	  "pushq %1;"
                	  "pushfq;"
			  "popq %%rax;"
			  "orq  $0x200,%%rax;"
                          "pushq %%rax;"
			  "pushq $0x2B;" //cs selector
                          "pushq %0;"
			  "movq $0x0, %%rsi;"
			  "movq $0x0, %%rdi;"
			  "iretq;" //final irteq - pops eip, cs, flags then goes to cs:ip
                      
			   ::  "r"(curr_task->rip),"r"(curr_task->ustack):"%rax");
				
}
     
//void print_task_structure(pcb *task)
int tarfs_lookup(char *tarfs,char *file,char **elf_hdr)
{
    char *p= tarfs;    
    while (!strcmp(p + 257, "ustar", 5)) 
    {
        int filesize = oct2bin(p+0x7c,11);
        if (!strcmp(p,file,strlen(file)+1)) 
	{
		
            *elf_hdr = p+ 512;
            return filesize;
	}
        p+=(((filesize+511)/512)+1)*512;
    }
    kprintf_k("No file found\n");
    return 0;
}
void check_path(char *path, char *new_path)
{
        //Base case : Root directory : '// pr /'
        if((path[0]=='/'&& path[1]=='/'&&path[2]=='\0') || (path[0] == '/'&&path[1]=='\0'))
        {
                *new_path='/';
                *(new_path+1)='\0';
                return;
        }
        // handling relative path
        if(first_occurence(path,'.')) //relative path
        {
                //get cwd : Current working directory
                if(*path == '.'&& *(path+1) =='/')
                {
                        char *cwd =(char *) page_alloc_k();
                        getcwd_syscall(cwd,256);
                        char *full_path = (char *)page_alloc_k();
			
                        strcat(full_path,cwd, path+1 );
                        get_full_path(full_path, new_path);
                        //now replace the dots in the path

                }
                else if(*path == '.' && *path+1 == '.' && *path+2== '/')
                {
                        char *cwd = (char *)page_alloc_k();
                        getcwd_syscall(cwd,256);
                        char *full_path =(char *)page_alloc_k();
                        strcat(full_path,cwd, path+2 );
                        //get prev directory from present path. 
                        get_full_path(full_path,new_path);
                	return;
		}
		else
		{
			
			get_full_path(path, new_path);
		}
        }
        else //absolute path 
        {
                //this is absolute path. 
                //handle the dots
                get_full_path(path, new_path);
		return;
        }
}

void get_full_path(char *path, char *new_path)
{       
        char s[10][50];
	for(int i=0;i<10;i++)
	{	
		for(int j=0;j<50;j++)
		{
				s[i][j]=0;
		}
	}
	int j=0; int top=0;
        for(int i=0;i<=strlen(path);i++)
        {      
                if(path[i]=='.' && path[i+1]=='.' && (path[i+2]=='/'||path[i+2]=='\0'))
                {       
                        if(s[top-1][0]!='/' && s[top-1][1]!='\0')
                        {      
                                if(top>0)
                                {
					for(int j=0;j<50;j++)
                                        	s[top-1][j]=0;       
                                        top--;
                                }
                        }
                        j=0;
			//if(path[i+2]=='\0')
			i+=2;
                }
                else if (path[i] !='/')
                {       
                        s[top][j++]=path[i];
                }
                else if(path[i]=='/' ||path[i]=='\0')
                {       
                        s[top][j++]='/';
                        s[top][j]='\0';
                        top++;
                        j=0;
                }
        }
        int k=0;
        for(int i=0;i<=top;i++)
        {      
		for(j=0;j<strlen(s[i]);j++)
                {      
			
			*(new_path+k)=s[i][j];
			k++;
		}
        }
	*(new_path+k)='\0';
        return;
}
int is_directory(char *path)
{
	if(path==NULL)
		return 1;
        if(path[0]=='/'||  path[1]=='\0')
                return 1;
        char *p=&(_binary_tarfs_start);

        while (p)
        {
                struct posix_header_ustar *ph =(struct posix_header_ustar*)(uint64_t)p;
                if(ph->name =='\0' )
                     break;
		int filesize = oct2bin(p+0x7c,11);
		if(!strcmp1(ph->name,path)&&(ph->name[strlen(ph->name)-1]!='/'))
			return 0;
                if(ph->name[strlen(ph->name)-1]=='/') // this says that its a directory as there's no file extension    
                {
                        int path_len= strlen(path);
                        if(path[strlen(path)-1]!='/')
			{	
				char new_path[path_len+2];
                      	  	strcpy(new_path,path);
                       	 	new_path[path_len]='/';
                        	new_path[path_len+1]='\0';
			
				if((ph->name[strlen(new_path)]=='/' || ph->name[strlen(new_path)]=='\0')&& !strcmp(new_path,(char *)(uint64_t)ph->name,strlen(new_path)))
                        	{
				

                                        kprintf_k("%s is the pathname\n",ph->name);
                                        if((char)ph->typeflag[0]=='5')
                                        return 1;
                        	}
			}
                	
			
			
		}
		
                p+=(((filesize+511)/512)+1)*512;
        }
        return 0;
}
void elf_read(Elf64_Ehdr *elf,vm_struct **mmap)
{	
	uint64_t phdr_offset = elf->e_phoff;
	Elf64_Phdr* phdr =(Elf64_Phdr*)((uint64_t)elf + phdr_offset);
	//vm_struct temp;
	int num_phdr = elf->e_phnum;
	vm_struct *list=NULL;
	
	for(int i=0;i<num_phdr;i++)
	{
		if(phdr->p_type==1)
		{
			vm_struct *temp =(vm_struct *)page_alloc_k();
	                temp->flags= phdr->p_flags;
			//virtual address
        	        temp->address = (phdr->p_vaddr);
                	temp->size = phdr->p_memsz;
            	        temp->next= NULL;	
			temp->fp = (uint64_t)elf+ (uint64_t)(phdr->p_offset);
			temp->filesize = phdr->p_filesz; 				
			temp->type=0;
			if(list==NULL)
                        	list= temp;
                	else
                        {
				temp->next=list;
				list=temp;
			}
		}	
		phdr++;	
	}
	vm_struct *heap= (vm_struct *)page_alloc_k();
	
	heap->address= KERNBASE -((PAGESIZE)*1000);
	heap->type= 2;
	heap->size = PAGESIZE;
	if(list)
	{
		heap->next =list;
		list=heap;
	}
	
	vm_struct *stack = (vm_struct *)page_alloc_k();//;KERNBASE - (4096)*1000;
	
	stack->address = STACKTOP - PAGESIZE;
	stack->type =1;
	stack->size = PAGESIZE;
	 if(list)
        {
                stack->next =list;
                list=stack;
        }

	*mmap=list;
	return;
}

uint64_t elf_load(char *filename,vm_struct **list)
{
	char *elf_hdr;
	int filesize;
	filesize = tarfs_lookup(&(_binary_tarfs_start),filename,&elf_hdr );
	//kprintf_k("%d is the size of the file \n",filesize);
	
	Elf64_Ehdr *elf =(Elf64_Ehdr *)elf_hdr;
	if(filesize>0)	
		elf_read( (Elf64_Ehdr *)elf , list);	
	return elf->e_entry;

}
/*
void print_task_structurepcb *task)
{
	kprintf_k("This is to print the task structure \n ");
				
	return;
}*/
void create_task(char *filename)
{
	
	kprintf_k("Filename : %s \n",filename);
	pcb *task = new_task();
	run_queue_add(task);		
	//memset((uint64_t)task, 0, sizeof(struct pcb));
	//task->vm_head=(vm_struct *) page_alloc_k();
      	//memset((uint64_t)task->mm, 0, sizeof(struct mm_struct));
	task->ustack =  STACKTOP ;
//      	task->prev_task=NULL; 	
	//kprintf_k("%s  is the filename in createtask \n",filename);		
	vm_struct *list=NULL;	
	task->rip = elf_load(filename,&list);
	
	task->vm_head =list;
	curr_task= task;
	char path[2]="/";
	strcpy(task->cwd,path);
	switch_to_user_mode(1, 0, 0);	
	return;	
}
void exec_create_task(char *filename, uint64_t args_page, uint64_t stack_top)
{
	pcb* task = curr_task;
	vm_struct *list=NULL;	
	task->rip = elf_load(filename,&list);
        task->vm_head =list;
	char path[2]="/";
	strcpy(task->cwd,path);	 
        switch_to_user_mode(0, args_page, stack_top);
        return;

}	
uint64_t vm_search(uint64_t addr)
{
	vm_struct *vm =curr_task->vm_head;
	int l=0;
	int flag=0;
	while(vm!= NULL&&vm->type==0)
	{	
		uint64_t start = vm->fp;
		uint64_t end = start+ vm->size;
		
		 
		if(l==0&&addr>=start &&addr<=end)
			flag=1;
		else if(vm->next!=NULL && addr>=start &&addr<=end &&l!=0)
			flag=2;
		else if(addr>=start && addr<=end && vm->next==NULL)
			flag=3;
		
		l++;
		vm = vm->next;
		
	}
	return flag;
}

void page_fault_handler(uint64_t error_num)
{
	
	//int cow = copy_on_write(error_num);
	if (1)
	{
        	uint64_t cr2,cr3;
		cr3=get_cr3();
		cr2=get_cr2();
		uint64_t addr=cr2;
		struct vm_struct *vm=curr_task->vm_head;
       		while(vm!= NULL)
        	{
			uint64_t start = vm->address;
                        uint64_t end = start+ vm->size;
			if (addr >= start && addr < end)
			{
				if (error_num == 7)
				{
					copy_on_write(error_num);
					tlb_flush();
					return;
				}
				else
				{
					//stack
					uint64_t page = page_alloc_k();
                                	page_table_walk_k(((page-KERNBASE)&PAGEALIGN),(cr2&PAGEALIGN),(pml4)(cr3+KERNBASE),7);
					tlb_flush();
					return;
					
				}
			}
			/*
			else if(vm->type==1)
			{
				//stack;
				uint64_t start = vm->address;
                                uint64_t end = start+ vm->size;
				if( addr>=start && addr<=end)
				{	
					uint64_t page =page_alloc_k();
					memcpy(page,)
				}
			}*/
			vm=vm->next;
        	}
		
		kprintf_k("Segmentation fault : %p \n", cr2);
		while(1);
		exit_syscall(-1);
	}
	//while(1);
	return;

}

uint64_t copy_pagetables(uint64_t pml4_va)
{
#define VA(pa) ((pa)+KERNBASE)
#define PA(va) ((va)-KERNBASE)
        pml4* pml4_p= (pml4 * )(pml4_va);
        pml4* pml4_c= (pml4 *)page_alloc_k();
        memset((uint64_t)pml4_c, 0,PAGESIZE);
	//uint64_t p=get_paddr_user(STACKTOP-100,(pml4)pml4_va);
	//kprintf_k("Testing Stack pa : %p\n",p);	
	//kprintf_k("pml4_va : %p\n",pml4_va);	
        pml4_c[511] = pml4_p[511];
        for(int i=0;i<511;i++)
        {
                if((pml4_p[i])&1 )
                {
			//kprintf_k("%d is i\n",i);
                        pml4_c[i] = PA(page_alloc_k());
                        pdp *pdp_p = (pdp *)((VA(pml4_p[i]) & PAGEALIGN));
                        pdp *pdp_c = (pdp *)(VA(pml4_c[i]));
			//kprintf_k("pdp_p -  %p\n",pdp_p);
			pml4_c[i] |=7;
                        for(int j=0;j<512;j++)
                        {
                                if((pdp_p[j])&1 )
                                {
		                        //kprintf_k("%d is j\n",j);

                                        pdp_c[j] = PA(page_alloc_k());
                                        pd *pd_p = (pd *)((VA(pdp_p[j])) & PAGEALIGN);
                                        pd *pd_c = (pd *)(VA(pdp_c[j]));
					pdp_c[j] |=7;
                                        for(int k=0;k<512;k++)
                                        {
                                                if((pd_p[k])&1 )
                                                {
							//kprintf_k("%d is k\n",k);
                                                        pd_c[k] = PA(page_alloc_k());
                                                        pt *pt_p = (pt *)((VA(pd_p[k])) & PAGEALIGN);
                                                        pt *pt_c =(pt *)( VA(pd_c[k]));
                                    			pd_c[k]|=7;
				                        for(int l=0;l<512;l++)
                                                        {
								if(pt_p[l]!=0)
								{
									//kprintf_k("%d is l\n",l);	
									pt_c[l] = URP(pt_p[l]);
									pt_p[l] = URP(pt_p[l]);
									pt_p[l] = pt_p[l] |0x800;
									pt_c[l] = pt_c[l] |0x800;	
									tlb_flush();
									count_page(pt_p[l]+KERNBASE);
								}
							}
                                                }
                                        }

                                }


                         }

                    }

            }
	

	tlb_flush();
/*	uint64_t stack_child = page_alloc_k();
	memcpy((void *)stack_child,(void *)(STACKTOP-PAGESIZE),PAGESIZE);
	page_table_walk_k(stack_child-KERNBASE,STACKTOP-PAGESIZE,*(pml4_c+KERNBASE),0x07);
*/	return PA((uint64_t)pml4_c);

}
int copy_on_write(uint64_t errorno)
{
                // since errocode is 7., The exception is regarding COW
                //Now, we execute COW.
                //create a new page and copy the content of the current page to the new page allocated.
        
	        uint64_t cr2 =get_cr2();
                uint64_t cr3 =get_cr3();
		
		uint64_t pa_cr2 = get_paddr_user(cr2,(pml4)(cr3+KERNBASE));
                if(pa_cr2&(0x800)) /*chech if page is read-only and present*/
                {
                        if(get_reference_count(pa_cr2) ==1)
                        {

                                //update the Pagetable entry
                                page_table_walk_k((pa_cr2&PAGEALIGN),(cr2&PAGEALIGN),(pml4)(cr3+KERNBASE),7);
                                                        
                        }
			else
			{
				uint64_t page = page_alloc_k();
				uint64_t copy_from = (pa_cr2&PAGEALIGN) + KERNBASE;
				memcpy((void*)page, (void *)copy_from, PAGESIZE);
                                page_table_walk_k(((page-KERNBASE)&PAGEALIGN),(cr2&PAGEALIGN),(pml4)(cr3+KERNBASE),7);
				decrement_reference_count(pa_cr2);
			}
			return 1;

                }
    
	return 0;
}

 void copy_vmas(vm_struct *parent,vm_struct **child)
{
        vm_struct *temp =parent,*prev=parent;
        while(temp!=NULL)
        {

                vm_struct *vm  = (vm_struct *)page_alloc_k();
                memset((uint64_t)vm, 0,PAGESIZE);
                vm->address = temp->address;
                vm->size= temp->size;
                vm->type =temp->type;
                vm->fp = temp->fp;
                vm->filesize =temp->filesize;
                if(temp==parent)
                {
                        vm->next=NULL;
                        *child = vm;
                        prev = vm;
                }
                else
                {
                        
			prev->next=vm;
                        vm->next=NULL;
			prev = prev->next;
                }
                temp=temp->next;
        }
        return;

}

uint64_t fork_syscall()
{
	pcb *parent= curr_task;	
        pcb *child= new_task();

        memcpy(child->kstack, parent->kstack, 512*8);
        child->pml4_t = copy_pagetables(parent->pml4_t + KERNBASE);
        child->parent_pid = parent->pid;
	child->ustack = parent->ustack;
	child->rip= parent->rip;
        strcpy(child->cwd,parent->cwd);
	vm_struct *list = (vm_struct *)page_alloc_k();
        memset((uint64_t)list,0,PAGESIZE);
        copy_vmas(parent->vm_head, &list);
        child->vm_head = list;
	child->kstack[511] = 92736;

	uint64_t* rsp;	
         __asm__ volatile("movq %%rsp,%0":"=b"(rsp));
	child->rsp = (uint64_t)&child->kstack[510 - (&parent->kstack[510] - rsp) -15 +9];

	run_queue_add(child);	
	if(curr_task->pid==parent->pid)
	{
		return child->pid;
	}
	
	return 0;
}
void sleep_syscall(int sec)
{
	curr_task->state=SLEEPING_P;
        curr_task->sleep_sec=sec;
	schedule(); 		
}
char globalfname[100];
void exec(char *filename,char *args[],char *envp[])
{
#define BYTEALIGN(addr) (((addr)/8)*8)
	
	uint64_t arg_page = page_alloc_k();
	
	int envp_count=0;
	
	while(envp && envp[envp_count]!=NULL)
		envp_count++;
	int args_count=0;
	while(args && args[args_count]!=NULL)
                args_count++;
	
	char *ptr = (char*) (arg_page+PAGESIZE-50);
	char *content_ptr=(char*) (STACKTOP-50);
	
	for(int i=envp_count-1;i>=0;i--)
	{
		int len = strlen(envp[i]);
		ptr-=len+1;
		strcpy(ptr,envp[i]);
		
	}
	for(int i=args_count-1;i>=0;i--)
        {
                int len = strlen(args[i]);
                ptr-=len+1;
                strcpy(ptr,args[i]);
        
        }
	ptr -=24;
	ptr = (char *)BYTEALIGN((uint64_t)ptr);
	//Nowgive a NULL to indicate end of envps
	uint64_t *ptr1;	
	ptr1=(uint64_t*)ptr;
	*ptr1=0;
	ptr1--;
	for(int i=envp_count-1;i>=0;i--)
        {
		*ptr1= (uint64_t)(content_ptr-(strlen(envp[i])+1));
		ptr1--;
		content_ptr-=strlen(envp[i]) + 1; 
        }
	*ptr1=0;
	ptr1--;
	 for(int i=args_count-1;i>=0;i--)
	{
		*ptr1= (uint64_t)(content_ptr-(strlen(args[i])+1));
		ptr1--;
		content_ptr-=strlen(args[i])+1;
	}
	*ptr1=args_count;

	//TODO: Argments for exec should be done
	
	strcpy(globalfname, filename);
	clear_process_mem(curr_task);
	exec_create_task(globalfname, arg_page, STACKTOP - (arg_page+PAGESIZE-((uint64_t)ptr1)));
 	return;	
}
void update_sleep_tasks()
{
	for(int i=0;i<PROCESS_COUNT;i++)
	{
		if(all_tasks[i].state==SLEEPING_P)
		{
			all_tasks[i].sleep_sec--;
			if(all_tasks[i].sleep_sec==0)
				run_queue_add(&all_tasks[i]);
		}
	}
	return;
}






//DIRECTORIES
void initialise_files(pcb *task)
{
	for(int i=0;i<3;i++)
	{
		task->files[i].state=ALLOCATED_F;
			
		if(i==0) strcpy(task->files[i].file_name,"stdin");
		if(i==1) strcpy(task->files[i].file_name,"stdout");
		if(i==2) strcpy(task->files[i].file_name,"stderr");
	}
	for(int i=3;i<FILE_COUNT;i++)
	{
		task->files[i].state=UNALLOCATED_F;
		
	}
	return;

}
int is_file(char *path)
{
	if(path==NULL)
                return 0;
	char *p=&(_binary_tarfs_start);
	while (1)
        {
                struct posix_header_ustar *ph =(struct posix_header_ustar*)(uint64_t)p;
                if(strlen(ph->name)==0)
                        break;
		int filesize = oct2bin(p+0x7c,11);
		if(!strcmp(path, ph->name,strlen(path))) 
                {
			if((char)(ph->typeflag[0])=='0'||(char)ph->typeflag[0]=='\0')
				return 1 ;
                }
                p+=(((filesize+511)/512)+1)*512;
        }
	return 0;

	
}
int get_file_ptr(char *path,char **fp)
{
        if(path==NULL)
                return 0;
        char *p=&(_binary_tarfs_start);
        while (1)
        {
                struct posix_header_ustar *ph =(struct posix_header_ustar*)(uint64_t)p;
                int filesize = oct2bin(p+0x7c,11);

                if(!strcmp(path, ph->name,strlen(path)))
                {
                        if((char)ph->typeflag[0]=='0'||(char)ph->typeflag[0]=='\0')
                {
				*fp=(char *)((uint64_t)ph+512);
		         	return filesize ;
                }
		}
                p+=(((filesize+511)/512)+1)*512;
        }
        return 0;

}
int open_syscall(char *path,int flags)
{
	if(path==NULL)
		return -1;
        char new_path[256];
        check_path(path, new_path);
	if(is_file(new_path))
	{	
		int fd= get_file_des(curr_task);
		strcpy(curr_task->files[fd].file_name,new_path);
		int filesize=0;
		char *file_ptr;
		filesize =get_file_ptr(new_path,&file_ptr);		
		curr_task->files[fd].start_address= (uint64_t)file_ptr;
		curr_task->files[fd].file_size=filesize;
		curr_task->files[fd].offset = curr_task->files[fd].start_address;	
		return fd;
	}
	kprintf_k("Error in opening the file\n");

	return -1;	
}
int  close_syscall(int fd)
{
	if(fd>=3 && fd<FILE_COUNT)
	{
		curr_task->files[fd].state = UNALLOCATED_F;
		return fd;
	}
	else 
	{
		kprintf_k("Error in closing the file\n");
		return -1;
	}
}
int get_file_des(pcb *task)
{
	for(int i=3;i<FILE_COUNT;i++)
	{	
		if(task->files[i].state==UNALLOCATED_F)
		{	
			task->files[i].state=ALLOCATED_F;
			return i;
		}
	}
	kprintf_k("No more FILEdescriptors available for this process\n");
	return -1;	
}


uint64_t  opendir_syscall(char *path,int flags)
{
        char new_path[256];
        check_path(path, new_path);
        if(is_directory(new_path))
        {
                //get a free filedes
                for(int i=0;i<FILE_COUNT;i++)
                {
                        if(curr_task->dirs[i].state==UNALLOCATED_D)
                        {
                                strcpy((curr_task->dirs[i].dir_name),new_path);
				
                                curr_task->dirs[i].offset= (uint64_t)&(_binary_tarfs_start);
                               	return (i);
                        }
                }
        }
	kprintf_k("No directory\n");
        return -1;
}

uint64_t readdir_syscall(int fd,int flags,char *buf)
{

        int dir_len= strlen(curr_task->dirs[fd].dir_name);
        char dir_name[dir_len];
        strcpy(dir_name, curr_task->dirs[fd].dir_name);
        char *p=(char *)curr_task->dirs[fd].offset;
	int flag=0;
        int all=0;
		
	if(dir_name[0]=='/'&&dir_name[1]=='\0') all=1;
	while(1)
        {
                struct posix_header_ustar *ph =(struct posix_header_ustar*)(uint64_t)p;
                if(!strlen(ph->name)) break;
		int filesize = oct2bin(p+0x7c,11);
                char dir_content[256];
		for(int i=0;i<256;i++)	dir_content[i]=0;
		if(prestring(dir_name,ph->name)||all)
                {
			if(all)
			{		
				int i=0;
                                for(i=0;i<strlen(ph->name);i++)
                                {
                                        if(ph->name[i]=='/')
                                                break;
                                        buf[i]=ph->name[i];
                                }
                                buf[i]='\0';
				p+=(((filesize+511)/512)+1)*512;
                                curr_task->dirs[fd].offset=(uint64_t)p;
                                return 1;
			}	
                        get_remaining_string(dir_content,ph->name,dir_name);
			if( first_occurence(dir_content,'/')&&strlen(dir_content)>=2)
                        {
                                //its a directory
				int i=0,l=0;
				if(dir_content[i]=='/') l=1;	
                        	for(i=l;i<strlen(dir_content);i++)
				{
					if(dir_content[i]=='/')
						break;
					buf[i]=dir_content[i];		
				}
				buf[i]='\0';
				flag=1;
			}
                	
                	else if(strlen(dir_content)>=2)
                	{
                  		strcpy(buf,dir_content);
				flag=1;	
                	}
			if(flag)
			{
				p+=(((filesize+511)/512)+1)*512;
				curr_task->dirs[fd].offset=(uint64_t)p;
				return 1;
			}
       		}
		p+=(((filesize+511)/512)+1)*512;

	}
	return 0 ;
}

uint64_t write_syscall(uint64_t fd, char *buf, uint64_t count)
{
	uint64_t i=0;
        if(fd!=1&&curr_task->files[fd].state==ALLOCATED_F )
        {
		char *p = (char *)curr_task->files[fd].offset;
		while(count-- && buf[i]!='\0')
		{
			*p=*buf;
			buf++;p++;
			i++;
					
		}
        }
        else if(fd==1)
	{
		while(count-- && buf[i]!='\0')
        	{
                	kprintf_k("%c",buf[i]);
                	i++;
        	}
		return i;
	}
	else
		kprintf_k("Error in writing to the file\n");
	return -1;
}
uint64_t read_syscall(int fd,char *buf,int count)
{
        if(fd==0)
	{	
		int count1;
        	count1=consumer_read(buf);
		return count1;
	}
	else
	if(fd==1 ||fd == 2)
	{
		kprintf_k("Error in reading. Cant read from stdout/stderr\n ");
		return -1;
	}
	if(!(curr_task->files[fd].state==ALLOCATED_F))
		return -1;
	if(count<0)return -1;
	
	char *p = (char *)curr_task->files[fd].offset;
	char *end = (char *)(curr_task->files[fd].file_size+curr_task->files[fd].start_address);
	if(count<=((uint64_t)end-(uint64_t)p))
	{
		while(count--)
		{
			*buf=*p;
			p++;buf++;
		}
		*buf='\0';
		curr_task->files[fd].offset = (uint64_t)p+count;
		return count;
	}
	else
	{
		count=0;	
		while(p<=end)
		{
			count++;
			*buf=*p;
                        p++;buf++;
		}
		*buf='\0';
		curr_task->files[fd].offset = (uint64_t)p+count;
	}
		
	return count;	
}		




