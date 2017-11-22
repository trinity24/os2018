#ifndef __TASK_H__
#define __TASK_H__

typedef struct pcb{
  uint64_t kstack[128];
  uint64_t pid;
  uint64_t rsp;
  enum {RUNNING, SLEEPING, ZOMBIE} state;
  int exit_status;
} pcb;

#endif
