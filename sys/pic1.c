#include <sys/pic.h>
void PIC_sendEOI(unsigned char irq)
{
        if(irq >= 8)
                outb(PIC2_COMMAND,PIC_EOI);

        outb(PIC1_COMMAND,PIC_EOI);
}
void PIC_remap(int interrupt_offset_PIC1, int interrupt_offset_PIC2)
{
        unsigned char a1, a2;
    
        a1 = inb(0x21);                        // save masks
        a2 = inb(0xA1);

        outb(0x20, 0x11);  // in the ICW1 - the init bit(bit 4) and cascaded mode(bit 0) 
        io_wait();
	
        outb(0xA0, 0x11);
        io_wait();

        outb(0x21, interrupt_offset_PIC1);                 //  This is the interrupt vector offset for the two pics
        io_wait();
        outb(0xA1, interrupt_offset_PIC2);                 
        io_wait();

        outb(0x21, 0x4);                       // This tells to which IRQ of the mater pic is the slave conected
        io_wait();
        outb(0xA1, 0x2);                       // slave pic's identity : pic2 -2
        io_wait();

        outb(0x21,0x01);
        io_wait();
        outb(0xA1, 0x01);
        io_wait();

	
	
	
        outb(0x21, a1);   // restore saved masks.
        outb(0xA1, a2);
}
