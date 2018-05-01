/*
pushing 15 registers rsi,rdi,rsp,rbp,rax,rbx,rcx,rdx,r8-15
so, each registers are at 0x8, 1x8 through 14*8
so, at 15*18 we will have the error code. 

*/


.text

.global context_switch_routine,isr32, isr33, isr80, isr14

.macro pop_registers_saving_rax
   popq %r15
   popq %r14
   popq %r13
   popq %r12
   popq %r11
   popq %r10
   popq %r9
   popq %r8
   popq %rcx /*instead of popping into rax, we pop into some register, say rcx*/
   popq %rcx
   popq %rdx
   popq %rbx
   popq %rbp
   popq %rsi
   popq %rdi
.endm


.macro push_registers
   pushq %rdi
   pushq %rsi
   pushq %rbp
   pushq %rbx
   pushq %rdx
   pushq %rcx
   pushq %rax
   pushq %r8
   pushq %r9
   pushq %r10
   pushq %r11
   pushq %r12
   pushq %r13
   pushq %r14
   pushq %r15
.endm

.macro pop_registers
   popq %r15
   popq %r14
   popq %r13
   popq %r12
   popq %r11
   popq %r10
   popq %r9
   popq %r8
   popq %rax
   popq %rcx
   popq %rdx
   popq %rbx
   popq %rbp
   popq %rsi
   popq %rdi
.endm
//here rsi is next_task and rdi is current_task
//we have to save context of current task. So, save its CPU registers(all general purpose) 
//by pushing them on to the stack
// now save the rsp on to the pcb
//now get next process onto the CPU by getting its cpu_Regs(from PCB) to the CPU
context_switch_routine:
   push_registers
   movq %rsp, (%rdi)
   movq %cr3,%rax
   movq %rax, 8(%rdi)
   
   movq (%rsi) , %rsp
   movq 8(%rsi), %rax
   movq %rax, %cr3
   pop_registers
   retq   
   
   

/* Timer Handler */
isr32:
   cli
   push_registers
   call timer
   pop_registers
//   sti
   iretq


/*Keyboard Handler*/
isr33:
   cli
   push_registers
   call kb_interrupt_handler
   pop_registers
//   sti
   iretq

/*Syscall Handler*/
isr80:
   cli
   push_registers
   movq %rsp,%rdi
   call syscallHandler
   pop_registers_saving_rax
//   sti
   iretq



/*PageFault Handler*/
isr14:
   cli
   push_registers
   movq 120(%rsp),%rdi
   call page_fault_handler
   pop_registers
   add $8, %rsp
//   sti
   iretq













