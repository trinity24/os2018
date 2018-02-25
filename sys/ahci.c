#include <sys/kprintf1.h>
#include <sys/ahci.h>
typedef uint8_t BYTE;
#define AHCI_BASE 0x500000        // 4M
//#define BYTE 

static int check_type(hba_port_t *port);
void start_cmd(hba_port_t *port);
void stop_cmd(hba_port_t *port);
hba_mem_t *abar;
void memset1(void *p,char c,int b)
{
        int i=0;
        char * ptr = (char *)p;
        for(i=0;i<b;i++)
        {
                *ptr=c;
                ptr++;

        }
        return;
}
void sleep(int i)
{
	for(int j=0;j<i;j++);
		
	
}
int probe_port(hba_mem_t  *abar)
{
	// Search disk in impelemented ports
	DWORD pi = abar->pi;
	int i = 0;
	while (i<32)
	{
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				kprintf("SATA drive found at port %d\n", i);
				return i;
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				kprintf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				kprintf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				kprintf("PM drive found at port %d\n", i);
			}
			else
			{
				kprintf("No drive found at port %d\n", i);
			}
		}
 
		pi >>= 1;
		i ++;
	}
	return -1;
}
 
// Check device type
static int check_type(hba_port_t *port)
{
	DWORD ssts = port->ssts;
 
	BYTE ipm = (ssts >> 8) & 0x0F;
	BYTE det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

void port_rebase(hba_port_t *port, int portno)
{      
//	abar->ghc=(DWORD)(0x80000003);
//	kprintf("%x is the port \n",port);
        stop_cmd(port); // Stop command engine
	//kprintf("stop working\n");        
        // Command list offset: 1K*portno
        // Command list entry size = 32
        // Command list entry maxim count = 32
        // Command list maxim size = 32*32 = 1K per port
        port->clb = AHCI_BASE + (portno<<10);
//        port->clbu = 0;
        
	memset1((void*)(port->clb), 0, 1024);
        
        // FIS offset: 32K+256*portno
        // FIS entry size = 256 bytes per port
        port->fb = AHCI_BASE + (32<<10) + (portno<<8);
  //      port->fbu = 0;
        memset1((void*)(port->fb), 0, 256);
        
        // Command table offset: 40K + 8K*portno
        // Command table size = 256*32 = 8K per port
        hba_cmd_header_t *cmdheader = (hba_cmd_header_t *)(port->clb);
        for (int i=0; i<32; i++)
        {       
                cmdheader[i].prdtl = 8; // 8 prdt entries per command table
                                        // 256 bytes per command table, 64+16+48+16*8
                // Command table offset: 40K + 8K*portno + cmdheader_index*256
                cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
    //            cmdheader[i].ctbau = 0;
                memset1((void*)cmdheader[i].ctba, 0, 256);
        }
//	kprintf("port rebasing done and starting the port\n");
	port->sctl =0x301; 
	sleep(10000);
	port->sctl= 0x300;
	      
        
	start_cmd(port);        // Start command engine
	
	//port->sctl =0x301;
        //sleep(1000);
        //port->sctl= 0x300;
	port->cmd|= 0x2000000F;
	sleep(1000) ;
	
          

}

// Start command engine
void start_cmd(hba_port_t *port)
{	
  while (port->cmd & HBA_PxCMD_CR);

    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST; 
    //start_cmd(port);*/    // Start command engine
    port->ie = 0xffff;   
    port->serr_rwc = 0xffff;
}
 
// Stop command engine
// Stop command engine
void stop_cmd(hba_port_t *port)
{
//	kprintf("%d is port_cmd\n",port->cmd);

	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
//	kprintf("%d is tje port_cmd\n",port->cmd); 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		 if((port->cmd & HBA_PxCMD_CR))
                        {
                                kprintf("cr dash ");
                                continue;
                        }
		if (port->cmd & HBA_PxCMD_FR)
			{
				 kprintf("fr dash ");
				continue;
			}
		break;
	}
 
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
}
int find_cmdslot(hba_port_t *port);
 
int read(hba_port_t *port, DWORD startl, DWORD starth, DWORD count, WORD *buf)
{
	port->is_rwc = 0xffff;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return 0;
//	kprintf("%x is port->clb\n",port->clb);
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(DWORD);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count
	cmdheader->c = 1; 
	cmdheader->p= 1;
//	kprintf("%x is cmdheader->ctba\n",cmdheader->ctba);
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	
	memset1(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
 	int i=0;
	// 8K bytes (16 sectors) per PRDT
	/*for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}*/
	// Last entry
	
	
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 0;
 	
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (BYTE)startl;
	cmdfis->lba1 = (BYTE)(startl>>8);
	cmdfis->lba2 = (BYTE)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (BYTE)(startl>>24);
	cmdfis->lba4 = (BYTE)starth;
	cmdfis->lba5 = (BYTE)(starth>>8);
 
        cmdfis->count = count ;
	//kprintf("%d is count\n",cmdfis->count);
		// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return 0;
	}
//	kprintf("slot is %d\n",slot);
	port->ci = 1<<slot;	// Issue command
	// Wait for completion
	
	while (1)
	
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			{
	//		kprintf("noway!\n");
			break;
			}
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return 0;
		}	
		
	}
 
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return 0;
	}
	 
	return 1;

}
int write(hba_port_t *port, DWORD startl, DWORD starth, DWORD count, WORD *buf)  
{
       port->is_rwc = 0xffff;              // Clear pending interrupt bits
       // int spin = 0;           // Spin lock timeout counter
        int slot = find_cmdslot(port);
        if (slot == -1)
                return 0;
//	kprintf("%d is the slot in write\n",slot);
        uint64_t addr = 0;
 //       addr = (((addr | 0) << 32) | port->clb);
        hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)(port->clb);
 
        cmdheader += slot;
       cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(DWORD);     // Command FIS size
        cmdheader->w = 1;               // Read from device
        cmdheader->c = 1;               // Read from device
     cmdheader->p = 1;               // Read from device
        // 8K bytes (16 sectors) per PRDT
	cmdheader->prdtl=1;
        //cmdheader->prdtl = (WORD)((count-1)>>4) + 1;    // PRDT entries count
 
        addr=0;
        addr=(((addr | 0)<<32)|cmdheader->ctba);
        hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(  addr);
        
        int i = 0; 
        // 8K bytes (16 sectors) per PRDT
        /*for (i=0; i<cmdheader->prdtl-1; i++)
        {
               cmdtbl->prdt_entry[i].dba = (DWORD)((uint64_t)buf & 0xFFFFFFFF);
                cmdtbl->prdt_entry[i].dbc = 8*1024-1;     // 8K bytes
                cmdtbl->prdt_entry[i].i = 0;
                buf += 4*1024;  // 4K words
                count -= 16;    // 16 sectors
       }*/
        
        // Last entry

        cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
        cmdtbl->prdt_entry[i].dbc = count << 9 ;   // 512 bytes per sector
        cmdtbl->prdt_entry[i].i = 0;
        
	
        // Setup command
        fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
        cmdfis->fis_type = FIS_TYPE_REG_H2D;
        cmdfis->c = 1;  // Command
        cmdfis->command = ATA_CMD_WRITE_DMA_EX;
 
        cmdfis->lba0 = (BYTE)startl;
        cmdfis->lba1 = (BYTE)(startl>>8);
        cmdfis->lba2 = (BYTE)(startl>>16);
        cmdfis->device = 1<<6;  // LBA mode
 
        cmdfis->lba3 = (BYTE)(startl>>24);
        cmdfis->lba4 = (BYTE)starth;
        cmdfis->lba5 = (BYTE)(starth>>8);
 
        cmdfis->count = count;
      port->ci = 1<<slot;    // Issue command
        // Wait for completion
        while (1)
        {
                if ((port->ci & (1<<slot)) == 0) 
                        break;
                if (port->is_rwc & HBA_PxIS_TFES)   // Task file error
                {
                        kprintf("Read disk error\n");
                        return 0;
                }
        }
        if (port->is_rwc & HBA_PxIS_TFES)
        {
                kprintf("Read disk error\n");
                return 0;
        }
        return 1;
}
 
// Find a free command list slot
int find_cmdslot(hba_port_t *port)
{
	// If not set in SACT and CI, the slot is free
	DWORD slots = (port->sact | port->ci);
	hba_mem_t *abar1 = (hba_mem_t *)abar;
	int cmdslots= (abar1->cap & 0x0f00)>>8 ; 
	for (int i=0; i<cmdslots; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	kprintf("Cannot find free command list entry\n");
	return -1;
}

void ahci(uint32_t abar1)
{
	int i=0;
	//abar=(uint64_t)abar1;
	abar = (hba_mem_t *)(uint64_t)abar1;
	int port_num = probe_port(abar);
	
//	hba_port_t port=abar->ports[port_num];
	//memset1((void *)port,0,sizeof(hba_port_t));
	//kprintf("%x os the port\n",port);
//	port=abar->ports[port_num];
	abar->ghc=(uint32_t)(0x80000003);
//	kprintf("%x is ssts\n", port.ssts);
	port_rebase(&abar->ports[port_num],port_num);
		
	WORD *buffer =(WORD*)0x6000F;
	WORD *buffer1=(WORD*)0x7000F;
	uint32_t l=0,h=0,count=1;	
	memset1((void *)buffer,0,512);
	memset1((void *)buffer1,0,512);
//	WORD *buf=buffer;	
	for( i=0;i<512;i++)
        {
		*buffer=i;
		buffer++;
	}
	buffer=(WORD*)0x6000F;
	for(int i=0;i<8;i++)
		write(&abar->ports[port_num],l ,h,count,buffer);
	memset1((void *)buffer1,0,512);
	memset1((void *)buffer,0,512);
	
	     for(i=0;i<512;i++)
        {
                kprintf("%d ",*buffer1);
                buffer1++;
        }	
	read(&abar->ports[port_num],l,h,count,buffer1);
	
	//memset1(buffer,0,512);
	for(i=0;i<512;i++)
	{
		kprintf("%d ",*buffer1);
		buffer1++;
	}
	
	return;
}

