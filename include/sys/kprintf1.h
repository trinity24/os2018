#ifndef __KPRINTF1_H
#define __KPRINTF1_H
void clearscreen();
void kprintf(const char *fmt, ...);
void idt_initialize();
void scroll_up();
void kprintf_k(const char *fmt, ...);
#endif
