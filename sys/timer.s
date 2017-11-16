.text

.global isr32, isr33
/*
timer:
   movq 8(%rsp),%rax
   add $1 , %rax
   movq %rax, 8(%rsp)
   ret 	*/
isr32:
   cli
   push %rdi
   push %rsi
   push %rbp
   push %rsp
   push %rbx
   push %rdx
   push %rcx
   push %rax                //# ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
   //movl %eax, %ds             // # ; Lower 16-bits of eax = ds.
   //push %eax             //   # ; save the data segment descriptor
  // movl  %eax,0x10      // #  ; load the kernel data segment descriptor
  // movl  %ds, %eax
  // movl  %es, %eax
  // movl  %fs, %eax
  // movl  %gs, %eax
   call timer
   //pop %eax      //  # ; reload the original data segment descriptor
  // movl %ds,%eax
  // movl %es, %eax
  // movl %fs,%eax
  // movl  %gs, %eax
   pop %rax
   pop %rcx
   pop %rdx
   pop %rbx
   pop %rsp
   pop %rbp
   pop %rsi
   pop %rdi          //         # ; Pops edi,esi,ebp...
 //  add $8,%esp   //  # ; Cleans up the pushed error code and pushed ISR number
   sti
   iretq

isr33:
   cli
   push %rdi
   push %rsi
   push %rbp
   push %rsp
   push %rbx
   push %rdx
   push %rcx
   push %rax                //# ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
   //movl %eax, %ds             // # ; Lower 16-bits of eax = ds.
   //push %eax             //   # ; save the data segment descriptor
  // movl  %eax,0x10      // #  ; load the kernel data segment descriptor
  // movl  %ds, %eax
  // movl  %es, %eax
  // movl  %fs, %eax
  // movl  %gs, %eax
   call kb_interrupt_handler
   //pop %eax      //  # ; reload the original data segment descriptor
  // movl %ds,%eax
  // movl %es, %eax
  // movl %fs,%eax
  // movl  %gs, %eax
   pop %rax
   pop %rcx
   pop %rdx
   pop %rbx
   pop %rsp
   pop %rbp
   pop %rsi
   pop %rdi          //         # ; Pops edi,esi,ebp...
 //  add $8,%esp   //  # ; Cleans up the pushed error code and pushed ISR number
   sti
   iretq
