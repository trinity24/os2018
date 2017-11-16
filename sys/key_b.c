//#include <system.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

unsigned char kb_code[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};		
int sht_flag=0;
extern void kb_interrupt_handler()
{
	
	unsigned char enter_key;
	uint16_t port=0x60;
//	__asm__("in 0x60 %0":"=r"(enter_key):);
	__asm__("inb %%dx,%%al":"=a" (enter_key):"d" (port));	
	 if(sht_flag==1)
        {	
		if(enter_key==0xF0)
                	{	
				sht_flag=0;
				return;
		        }
	}
	
	if(enter_key==39||enter_key==49)
	{
		sht_flag=1;
		return;
			
	}
	else
	{
		if(sht_flag==1)
		{
			//enter capital letter
			kprintf("%c",kb_code[enter_key]-32);
		}
		else
		{
			kprintf("%c",kb_code[enter_key]);
		}
	}
	return;
}
