#ifndef __TASK_H__
#define __TASK_H__
#define PAGEALIGN 0xfffffffffffff000
#define STACKTOP 0xfffffe0000000000
 typedef struct vm_struct{
    uint64_t flags;
    void *addr;
    
    uint64_t size;
    uint64_t address;
    
    uint64_t fp;
    uint64_t filesize;

   struct vm_struct *next;
    uint16_t type; //0 = for program header info, 1 for stack, 2 for heapsiz
  }vm_struct;

typedef struct mm_struct{
	
    vm_struct *mmap;
   uint64_t flags; 
 
} mm_struct;

typedef struct file
{
 short level ;
 short token ;
 short bsize ;
 char fd ;
 unsigned flags ;
 unsigned char hold ;
 unsigned char *buffer ;
 unsigned char * curp ;
 unsigned istemp; 
} file ;


typedef struct pcb{
  uint64_t *kstack;
 //[512] ;
  uint64_t pid;
  uint64_t rsp;
  enum {RUNNING, SLEEPING, ZOMBIE} state;
  int exit_status;
  uint64_t pml4_t;
  uint64_t ustack; 
  uint64_t rip;
  vm_struct *vm_head;
  uint64_t parent_index;

  uint64_t parent_pid,child_pid;
  
  struct pcb *parent,*child,*next_task,*prev_task;
  
uint64_t pid_parent, pid_child;
  /* filesystem information */
  //struct fs_struct *fs;
   /* open file information */
  file *files;
} pcb;

/*
struct mm_struct{
   struct vm_struct *mmap;
   uint64_t flags;	
};
 typedef struct  {
    uint64_t flags;
    void *addr;
    uint64_t size;
    vm_struct *next;
  }vm_struct;

*/
void switch_threads(pcb *t1, pcb *t2);
void function1();
void function2();
void context_switch();
void print_task_structure(pcb *task);
uint64_t elf_load(char *filename,vm_struct **list);

#endif
