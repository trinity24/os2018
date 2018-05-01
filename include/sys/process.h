
#ifndef _PROCESS_H
#define _PROCESS_H
#include <sys/defs.h>
//#include <sys/task.h>
#define PROCESSES_COUNT 1000
void initialise_pcb();
void page_fault_handler();

int oct2bin(char *str, int size);
#define PAGEALIGN 0xfffffffffffff000
int tarfs_lookup(char *tarfs, char *file, char **elf_hdr);
//void tarfs_search(char *filename);
//void elf_load(char *filename,vm_struct **list);
//void print_task_structure(pcb *task);
void create_task(char *filename);
void switch_to_user_mode();
//void memcpy(void *p1,void *P2,int size);
//uint64_t vm_search(uint64_t addr);
//void copy_on_write();
//void fork(pcb *parent);
int copy_on_write(uint64_t error_num);
//void copy_vmas (vm_struct *parent, vm_struct **child);
uint64_t get_cr3();
uint64_t get_cr2();
void page_fault_handler(uint64_t error_num);
void switch_to_user_mode();
void exec(char *filename);
void ps_syscall();
uint64_t kill_syscall(int process_id,int signal);
void adopt_orphan(int parent_id);
void exit_syscall(int status);
uint64_t wait_syscall(int *status);
uint64_t waitpid_syscall(int process_id, int *status);
void slacked_forever();
//void run_queue_add(pcb *task);
//void wait_queue_add(pcb *task);
//void clear_pcb(pcb task);
#endif
