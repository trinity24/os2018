#ifndef __TASK_H__
#define __TASK_H__
#define PAGEALIGN 0xfffffffffffff000
#define STACKTOP 0xfffffe0000000000
#define PROCESS_COUNT 8
#define FILE_COUNT 30
//extern uint64_t pid=0;
#define HEAD_PROCESS 0
#define NAME_MAX 256
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

struct file_des{
	char name[NAME_MAX];
	uint64_t offset;
	uint64_t start_address;
	uint64_t file_size;
	enum {UNALLOCATED_F, ALLOCATED_F} state;
};
typedef struct file_des FILE;

struct dir_des{
	char name[NAME_MAX];
	uint64_t offset;
	uint64_t start_address;
	uint64_t file_size;
	enum {UNALLOCATED_D, ALLOCATED_D} state;
};

typedef struct dir_des DIR;

typedef struct pcb{
  uint64_t rsp;
  uint64_t pml4_t;
  regs cpu_regs;
  uint64_t kstack[512];
  uint64_t pid;
  enum {UNALLOCATED_P, RUNNING_P, SLEEPING_P, ZOMBIE_P, ALLOCATED_P,WAITING_P, READ_SUSPEND_P} state;
  int exit_status;
  int sleep_sec;
  uint64_t ustack; 
  uint64_t rip;
 //TODO: Check if we can initialize here. to the root directory. 
  char cwd[1000]; 
  vm_struct *vm_head;
  uint64_t parent_pid;
  /* filesystem information */
  //struct fs_struct *fs;
   /* open file information */
  FILE files[FILE_COUNT];
  DIR dirs[FILE_COUNT];
} pcb;
pcb* curr_task;
uint64_t vm_search(uint64_t addr);

void initialise_tasks();
void task_switcher(pcb *current_task,pcb *next_task);
void switch_threads(pcb *t1, pcb *t2);
void function1();
void function2();
void context_switch_routine(pcb *current_task,pcb *next_task);
void context_switch();
void schedule();
void print_task_structure(pcb *task);
uint64_t elf_load(char *filename,vm_struct **list);
void copy_vmas (vm_struct *parent, vm_struct **child);
uint64_t fork_syscall();
void read_suspend_add(pcb *task);
void read_suspend_remove();
int consumer_read(char *buf);
void load_userprogram_content(vm_struct *list, uint64_t user_cr3,uint64_t kernel_cr3);
void run_queue_add(pcb *task);
void wait_queue_add(pcb *task);
void clear_pcb(pcb* task);
void idle();
void getpwd(char *curr_dir);
int chdir_syscall(char *path);
uint64_t get_pid();
uint64_t get_ppid();
char* getcwd_syscall(char *cwd,int size);
#endif
