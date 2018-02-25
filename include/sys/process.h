
#ifndef _PROCESS_H
#define _PROCESS_H
#include <sys/defs.h>
//#include <sys/task.h>

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

void page_fault_handler(uint64_t error_num);
void switch_to_user_mode();

#endif
