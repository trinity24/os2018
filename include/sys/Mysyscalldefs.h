
#ifndef _MYSYSCALLDEFS_H
#define _MYSYSCALLDEFS_H
#include <sys/defs.h>

uint64_t MyOpendef(uint64_t syscallNum,const char *file,uint64_t flag, uint64_t mode);
uint64_t Mygetdentsdef(uint64_t syscallNum,uint64_t fd, void * mydirent, uint64_t count);
uint64_t MyWritedef(uint64_t syscallNum,uint64_t fd, char * buf, uint64_t count);
uint64_t MyReaddef(uint64_t syscallNum,uint64_t fd,  char *buf,uint64_t count);
pid_t MyWaitpiddef(uint64_t syscallNum,pid_t pid,uint64_t *status, uint64_t options);
pid_t MyForkdef(uint64_t syscallNum);
uint64_t MyPipedef(uint64_t syscallNum,uint64_t *filedes);
#endif
