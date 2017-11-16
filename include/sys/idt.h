#ifndef _IDT_H
#define _IDT_H
void interrupt_call(uint64_t);
void initialize_idt();
void idt_set(uint8_t,uint64_t,uint16_t,uint8_t);
void default_intr();
//void load_idt(struct idt_ptr* ); 

#endif
