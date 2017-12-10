#include <sys/kprintf1.h>
#include <sys/defs.h>
#include <sys/process.h>
#include <sys/pic.h>
//#define cs 0x00180000000000
struct idt_entry  {
    uint16_t offset_1; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t ist;       // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t type_attribute; // type and attributes
   uint16_t offset_2; // offset bits 16..31
   uint32_t offset_3; // offset bits 32..63
   uint32_t zero;     // reserved
}__attribute__((packed));

//static volatile  char *time = (volatile char*)0xBC80;
int Mystrlen1(const char *s)
{       
        //printf("In strlen fucntion now: \n");
        int size=0;
        if(s==NULL)
                return 0;
        
        while(*s !='\0')
                {       
                        //printf("Now counting started into loop  %c \n",*s);
                        s++;
                        
                        size++;
                }
        return (size);
}
void base_print1 (unsigned long int value,char *finalstr,int iter,int base);
 void write_string1( int col, const char *string )
{
        int l=0;
        //printf("writing to the screen\n");
	//kprintf("\n%s",string);  
     volatile char *vi = (volatile char*)0xB8510;
    while( *string )
    {
	
        if(*string=='\n' )
        {

                vi=vi+(80-l)*2;
                string++;
                l=0;

        }
        else {
        l++;
        if(l > 80)
           l=0;
        *vi++ = *string++;
        *vi++ = col;
        }
    }
	return;
}
void print_time(int c)
{
	volatile char *v=(volatile char *)0xB8500;
	char *s="Time: ";
	while(*s)
	{
	
	 *v++ = *s++;
        *v++ = c;
	
	}
	return;
}
void put_time(int t)
{
	char s[200];
	base_print1(t,s,0,10);
	//kprintf("%s\n",s);
	write_string1(11,s);
	PIC_sendEOI(0);
}
void base_print1 (unsigned long int value,char *finalstr,int iter,int base)
{
        
    //int initial= iter;
    char s[10]; int i=0;
    int y=value%base;
   // int k; 
    unsigned int x=value; int len;
    while(x!=0)
    {       
            if(y>=10)
                    s[i++]='A'+y-10;
            else    
                    s[i++]=y+'0';
            x=x/base;
            y=x%base;
     
    }
    len=Mystrlen1(s);
         /*
    //l=Mystrlen(finalstr);
        remain_width= arg_width- len;
    while(remain_width>0)
    {   
        finalstr[iter++]=0+'0';
        
        remain_width--;
        }    */
   len=Mystrlen1(s);
    int j=0; 
    for(j=0;j<len;j++)
        finalstr[iter++]=s[len-j-1];
    //finalstr[iter]='\0';
    return ;
}

struct idt_ptr
{
    uint16_t  limit;
    uint64_t base;
}__attribute__((packed));
/*
extern void timer()
{
	static int countTimer=0;
	//static int sec=0,min=0,hour=0;
	countTimer++;
	put_time(countTimer);
	if(countTimer>18)
        {
            sec++;
            if(sec>60){
                sec=0;
                min++;
                if(min>60){
                        min=0;
                        hour++;
                        if(hour>24){
                                sec=0;min=0;hour=0;
                        }
                }
             }
        }               
        
        kprintf("%d:%d:%d",hour, min, sec);
*/
//extern void initialize_idt();
void interrupt_call(uint64_t num)
{
	__asm__ volatile ("INT $32");
	kprintf("Something is happening here");
	return;
}
extern inline void load_idt(struct idt_ptr *idt_pt)
{
	  __asm__ volatile ("lidt %0" ::"m"(*(idt_pt)));

}
//extern void isr14();
extern void isr32();
//void *memset(void *ptr, int x, size_t n);
void hollow_timer()
{
__asm__("iretq;");
//kprintf("came here");
}
//nn/static uint64_t countTimer=0;/*
/*

	*/

extern void isr80();
extern void isr33();
extern void isr14();
 void timer()
{ 
//	print_time(11);      
        static int countTimer=0,sec=0;
        countTimer++;
	kprintf_k("Seconds is %d\n",countTimer);
        if(countTimer>18)
        {   
		sec++;
	//	kprintf("Seconds is %d\n",sec);
         	countTimer=0;
//		print_time(11);
		put_time(sec);   
	}
	
	PIC_sendEOI(0);
}
 
void default_intr()
{
	
	kprintf_k("you are using other interrupts");
	while(1);
	
}
struct idt_entry idt_ent[256];
void idt_set(uint8_t index, uint64_t base, uint16_t selector, uint8_t type_att);

void initialize_idt()
{
//	kprintf("here in the initialise idt\n");
	struct idt_ptr idt; //this is idt_pointer. Now we have to load a register using this pointer address
	 // this is array of idt entries
	uint16_t lim=sizeof(idt_ent)-1;
	idt.limit=lim; //so, the table idt 
	idt.base=(uint64_t)(idt_ent);
	//kprintf("%d - %d are limit and base\n",idt.base,idt.limit);
	for(int i=0;i<256;i++)
	{
		idt_set(i,(uint64_t)(&default_intr),0x80,0x8E);
		
	}
	
	//memset(&idt_ent, 0, sizeof(idt_ent)*256); //setting all the values of the idt to 0; Now we can configure each required one starting from 32
	idt_set(32,(uint64_t )isr32,0x08,0x8E);	
	idt_set(33,(uint64_t)isr33,0x08,0x8E);
	idt_set(14,(uint64_t)isr14,0x08,0x8E);
	//idt_set(80, (uint64_t)isr80, 0x08, 0x8E);
	load_idt(&idt);	
	return;
		
}	

void idt_set(uint8_t index, uint64_t base, uint16_t selector, uint8_t type_att )
{
   uint16_t masking16 = 0xFFFF ;//16 bit masking
   uint32_t masking32 =0xFFFFFFFF; //32 bit masking
   idt_ent[index].offset_1= base & masking16;
   idt_ent[index].offset_2 = (base >> 16) & masking16;
   idt_ent[index].offset_3 = (base >> 32) & masking32;
   idt_ent[index].ist=0;
   idt_ent[index].selector = selector; 
   idt_ent[index].zero = 0;
   idt_ent[index].type_attribute = type_att;
   return;
}






