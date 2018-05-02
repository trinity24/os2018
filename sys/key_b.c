//#include <system.h>
#include <sys/defs.h>
#include <sys/kprintf1.h>
#include <sys/task.h>
#include <sys/pic.h>

#define BUFFER_SIZE 1000
char read_buff[BUFFER_SIZE];

int read_it=0,consumer_it=0,read_linecount=0;

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
		if(enter_key==0xaa || enter_key == 0xb6)
                	{	
				sht_flag=0;
				return;
		        }
	}
	
	//kprintf_k("%d\n", ((int)enter_key));
	if(enter_key==0x2a||enter_key==0x36)
	{
		sht_flag=1;
		return;
			
	}
	else
	{
		if (enter_key < 58)
		{
			if(sht_flag==1)
			{
				//enter capital letter
				kprintf_k("%c",kb_code[enter_key]-32);
				read_buff[read_it++]=kb_code[enter_key]-32;
			}
			else
			{
				kprintf_k("%c",kb_code[enter_key]);
				read_buff[read_it++]=kb_code[enter_key];
				read_it%=BUFFER_SIZE;
				if(kb_code[enter_key]=='\n')
				{
					
					read_linecount++;
					read_suspend_remove();
				}
					
			}
		}
	}

	PIC_sendEOI(1);
	return;
}

//TODO: Add a function 
//Read should call this function

int consumer_read(char *buff)
{
	int count =0;
	while(1)
	{
		if(read_linecount==0)
		{
			read_suspend_add(curr_task);
			schedule();
		}
		else
		{	
			while(read_buff[consumer_it]!='\n')
			{
				*buff=read_buff[consumer_it];
				consumer_it++;
				consumer_it%= BUFFER_SIZE;
				buff++;
				count++;
			}
			consumer_it++;
			consumer_it%= BUFFER_SIZE;
			read_linecount--;
			*buff= '\0';
			return count;
		}
	}
	return -1;
}
