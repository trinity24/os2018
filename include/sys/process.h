//#include <sys/defs.h>
//#include <sys/task.h>
void initialise_pcb();
int oct2bin(char *str, int size);
int tarfs_lookup(char *tarfs, char *file, char **elf_hdr);
//void tarfs_search(char *filename);
//void elf_load(char *filename,vm_struct **list);
//void print_task_structure(pcb *task);
void create_task(char *filename);
void switch_to_user_mode();
//void memcpy(void *p1,void *P2,int size);

void switch_to_user_mode();
