/*
pushing 15 registers rsi,rdi,rsp,rbp,rax,rbx,rcx,rdx,r8-15
so, each registers are at 0x8, 1x8 through 14*8
so, at 15*18 we will have the error code. 

*/


.text

.global context_switch_routine,isr32, isr33, isr80, isr14

.macro pop_registers_saving_rax
   pop %r15
   pop %r14
   pop %r13
   pop %r12
   pop %r11
   pop %r10
   pop %r9
   pop %r8
   pop %r8 /*instead of popping into rax, we pop into some register, say r8*/
   pop %rcx
   pop %rdx
   pop %rbx
   pop %rbp
   pop %rsi
   pop %rdi
.endm


.macro push_registers
   push %rdi
   push %rsi
   push %rbp
   push %rbx
   push %rdx
   push %rcx
   push %rax
   push %r8
   push %r9
   push %r10
   push %r11
   push %r12
   push %r13
   push %r14
   push %r15
.endm

.macro pop_registers
   pop %r15
   pop %r14
   pop %r13
   pop %r12
   pop %r11
   pop %r10
   pop %r9
   pop %r8
   pop %rax
   pop %rcx
   pop %rdx
   pop %rbx
   pop %rbp
   pop %rsi
   pop %rdi
.endm
//here rsi is next_task and rdi is current_task
//we have to save context of current task. So, save its CPU registers(all general purpose) 
//by pushing them on to the stack
// now save the rsp on to the pcb
//now get next process onto the CPU by getting its cpu_Regs(from PCB) to the CPU
context_switch_routine:
   push_registers
   movq %rsp, (%rdi)
   //TODO: For now removing
   //movq %cr3,%rax
   //movq %rax, 8(%rdi)
   
   movq (%rsi) , %rsp
   //movq 8(%rsi), %rax
   //TODO:for now removing cr3
   //movq %rax, %cr3
   pop_registers
   retq   
   
   

/* Timer Handler */
isr32:
   cli
   push_registers
   call timer
   pop_registers
   sti
   iretq


/*Keyboard Handler*/
isr33:
   cli
   push_registers
   call kb_interrupt_handler
   pop_registers
   sti
   iretq

/*Syscall Handler*/
isr80:
   cli
   push_registers
   movq %rsp,%rdi
   call syscallHandler
   pop_registers_saving_rax
   sti
   iretq



/*PageFault Handler*/
isr14:
   cli
   push_registers
   movq 120(%rsp),%rdi
   call page_fault_handler
   pop_registers
   sti
   iretq













