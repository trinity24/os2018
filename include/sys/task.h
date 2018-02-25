#ifndef __TASK_H__
#define __TASK_H__
#define PAGEALIGN 0xfffffffffffff000
#define STACKTOP 0xfffffe0000000000
//extern uint64_t pid=0;

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

typedef struct {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp, rip, flags, cr3;
} regs;
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
  
  uint64_t rsp;
  uint64_t pml4_t;
  regs cpu_regs;
 
  uint64_t *kstack;
  uint64_t pid;
  enum {RUNNING, SLEEPING, ZOMBIE} state;
  int exit_status;
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
//void copy_vmas(vm_struct *parent)
uint64_t vm_search(uint64_t addr);

void switch_threads(pcb *t1, pcb *t2);
void function1();
void function2();
void context_switch_routine(pcb *t1,pcb *t2);
void context_switch();
void print_task_structure(pcb *task);
uint64_t elf_load(char *filename,vm_struct **list);
void copy_vmas (vm_struct *parent, vm_struct **child);
void fork(pcb *parent);
void load_userprogram_content(vm_struct *list, uint64_t user_cr3,uint64_t kernel_cr3);

#endif
