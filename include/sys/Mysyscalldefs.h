
#ifndef _MYSYSCALLDEFS_H
#define _MYSYSCALLDEFS_H
#include <sys/defs.h>
uint64_t ret;
uint64_t MyOpendef(uint64_t syscallNum, char *file,int flags);
uint64_t Myclose(uint64_t syscallNum,int fd );
uint64_t Mygetdentsdef(uint64_t syscallNum,uint64_t fd, void * mydirent, uint64_t count);
uint64_t MyWritedef(uint64_t syscallNum,int fd, char * buf, uint64_t count);
uint64_t MyReaddef(uint64_t syscallNum,int fd,  char *buf,int count);
pid_t MyWaitpiddef(uint64_t syscallNum,pid_t pid,uint64_t *status, uint64_t options);

uint64_t Mywait(int syscallNum, int *status);
uint64_t Mywaitpid(int syscallNum, int pid, int *status);
uint64_t MyKill(int syscallNum,pid_t pid, int signal);
void Myps(int syscallNum);
void MyExit(int syscallNum,int status);
pid_t MyForkdef(uint64_t syscallNum);
void MyExecdef(uint64_t syscallNum,char *filename, char* args[], char* envp[]);
pid_t Myget_ppid();
uint64_t Myget_pid();
uint64_t MyPipedef(uint64_t syscallNum,uint64_t *filedes);
int Mychdirdef(int syscallNum,const char *path);
void* Mymalloc(uint64_t syscallNum, int size);
void* Mygetcwd(uint64_t syscallNum, char *buf,int size);
void Mysleepdef(uint64_t syscallNum, int sec);
#endif
