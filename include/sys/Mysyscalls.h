
#ifndef _MYSYSCALLS_H
#define _MYSYSCALLS_H
#include <sys/defs.h>
//TODO: Here in Myopen mode_t can be defined as given as RW,WR or something
pid_t MyWaitpid(pid_t pid,uint64_t *stat_add, uint64_t options);
/*uint64_t open(const char *file,uint64_t flag, uint64_t mode);
uint64_t write(uint64_t fd, char * buf, uint64_t count);
uint64_t read(uint64_t fd,  char *buf);
uint64_t kill(pid_t pid, int signal);
void ps();

uint64_t wait();
uint64_t waitpid();

int chdir(const char *path);

pid_t get_pid();
pid_t get_ppid();

pid_t fork();
uint64_t pipe(uint64_t *fildes);
void print(char *s);
void execve(char *filename);
*/


#endif
