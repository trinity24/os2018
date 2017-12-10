#include <sys/pci.h>	
#include <sys/kprintf1.h>
#include <sys/defs.h>
void write_to_pci( uint32_t address);
void write_to_data();
uint32_t pciConfig32 (uint8_t bus, uint8_t device, uint8_t func, uint8_t register_offset)
{
    uint32_t address;
    uint32_t bus_32  = (uint32_t)bus;
    uint32_t device_32 = (uint32_t)device;
    uint32_t func_32 = (uint32_t)func;
    uint32_t tmp = 0;
    address = (uint32_t)((bus_32 << 16) | (device_32 << 11) |(func_32 << 8) | (register_offset & 0xfc) | ((uint32_t)0x80000000));

    // now the address can be written to the pci_config address register
        //write to 0xCF8
    write_to_pci (address);
    
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    //now we have to read from 0xCFC as it is the data register
        tmp = (uint32_t)(read_from_pci());
        
    return (tmp);
}

uint16_t pciConfig (uint8_t bus, uint8_t device, uint8_t func, uint8_t register_offset)
{
    uint32_t address;
    uint32_t bus_32  = (uint32_t)bus;
    uint32_t device_32 = (uint32_t)device;
    uint32_t func_32 = (uint32_t)func;
    uint16_t tmp = 0;
    address = (uint32_t)((bus_32 << 16) | (device_32 << 11) |(func_32 << 8) | (register_offset & 0xfc) | ((uint32_t)0x80000000));
	
    // now the address can be written to the pci_config address register
        //write to 0xCF8
    write_to_pci (address);
    
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    //now we have to read from 0xCFC as it is the data register
        tmp = (uint16_t)((read_from_pci() >> ((register_offset & 2) * 8)) & 0xffff);
	
    return (tmp);
}
void  pciConfigWrite (uint8_t bus, uint8_t device,
                             uint8_t func, uint8_t register_offset)
 {
    uint32_t address;
    uint32_t bus_32  = (uint32_t)bus;
    uint32_t device_32 = (uint32_t)device;
    uint32_t func_32 = (uint32_t)func;
    address = (uint32_t)((bus_32 << 16) | (device_32 << 11) | (func_32 << 8) | (register_offset & 0xfc) | ((uint32_t)0x80000000));
    // now the address can be written to the pci_config address register
        //write to 0xCF8
    write_to_pci ( address);

		
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    //now we have to read from 0xCFC as it is the data register
    
write_to_data ();
	
	//tmp = (uint16_t)((read_from_pci() >> ((register_offset & 2) * 8)) & 0xffff);
    return;
}
void write_to_data()
{
	   uint16_t address = 0xCFC;
	   uint32_t add=0xA6000;
        __asm__("outl %1 ,%0 ;"
                        ::"d"(address),"a" (add));
	
        return;
}
void write_to_pci( uint32_t address)
{
	uint16_t add=0xCF8;	
        __asm__("outl %1 ,%0 ;"
			::"d"(add),"a" (address));
        return;

}

uint32_t get_AHCIBASE(uint8_t bus, uint8_t device)
{
	pciConfigWrite(bus,device,0,0x24);
       uint32_t tmp=pciConfig32(bus,device,0,0x24);
	kprintf("%x  the tmp \n",(uint32_t)(tmp) );
	
	return tmp;
}
uint32_t read_from_pci()
{
	uint32_t outadd;
        uint16_t add=0xCFC;
       	__asm__("inl %1 ,%0;" 
			:"=a"(outadd):"d"(add));
        return outadd;

}

uint16_t checkDevice(uint8_t bus_slot, uint8_t device_slot);

void checkAllBuses(void) {
     uint8_t bus;
     uint8_t device;
     for(bus = 0; bus < 255; bus++) {
         for(device = 0; device < 32; device++) {
             checkDevice(bus, device);
         }
     }
}

int checkAHCI(uint8_t bus, uint8_t device)
{
	//kprintf("in ahcichecking\n");
//	kprintf("%x is the deviceid and %x is the classid\n",checkDevice(bus, device),pciConfig32(bus,device,0,8));
            if(checkDevice(bus, device)==0x2922 &&  pciConfig32(bus,device,0,8)==0x01060102)	
		{				
			return 1;
		}
	return 0;
}
uint16_t checkDevice(uint8_t bus_slot, uint8_t device_slot)
{
        uint16_t vendor,device=0;
	vendor =pciConfig(bus_slot,device_slot,0,0);
        if(vendor!=0xFFFF)
        {
         //       kprintf("vendor name is %x \n", vendor);
                device=pciConfig(bus_slot,device_slot,0,2);
  //             kprintf("device name is %x \n",device);
        }       
        return device;
}
/*
 uint16_t checkVendor(uint8_t bus_slot, uint8_t device_slot)
 {
    uint16_t vendor, device;
   //  try and read the first configuration register. Since there are no 
   // vendors that == 0xFFFF, it must be a non-existent device. 
    if ((vendor = pciConfig(bus_slot,device_slot,0,0)) != 0xFFFF) {
       		device = pciConfig(bus_slot,device_slot,0,2);
//		kprintf("vendor name is %d \n", vendor);
//		 kprintf("device name is %d \n",device);
	}
    return vendor;
}
*/
