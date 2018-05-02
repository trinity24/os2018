#include <stdlib.h>

void _start(void) {
  
	//TODO: Main should also accept arguments and environment settings
	/*
	int argc=1;
	char *argv[10];
	char *envp[10];
	*/

	uint64_t rsp;
	__asm__ volatile("movq %%rsp,%0":"=b"(rsp));
	rsp += 8;	
	exit(main(*(int*)rsp,(char**)(rsp+8), 
                  (char**)(rsp+((*(int*)rsp) + 2)*8)));
}
