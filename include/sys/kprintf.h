#ifndef __KPRINTF_H
#define __KPRINTF_H
void clearscreen();
void kprintf(const char *fmt, ...);
void idt_initialize();
#endif
