
#ifndef _MYSYSCALLS_H
#define _MYSYSCALLS_H
#include <sys/defs.h>
//TODO: Here in Myopen mode_t can be defined as given as RW,WR or something
uint64_t MyOpen(const char *file,uint64_t flag, uint64_t mode);
uint64_t MyWrite(uint64_t fd, char * buf, uint64_t count);
uint64_t MyRead(uint64_t fd,  char *buf,uint64_t count);
pid_t MyWaitpid(pid_t pid,uint64_t *stat_add, uint64_t options);
pid_t MyFork();
uint64_t MyPipe(uint64_t *fildes);
void print(char *s);
#endif
