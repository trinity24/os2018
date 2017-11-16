
#include <sys/defs.h>
uint32_t pciConfig32 (uint8_t bus, uint8_t device,uint8_t func, uint8_t register_offset);
uint16_t pciConfig (uint8_t bus, uint8_t device,uint8_t func, uint8_t register_offset);
uint32_t read_from_pci();
uint16_t checkVendor(uint8_t bus_slot, uint8_t device_slot);
void checkAllBuses(void);
void pciConfigWrite(uint8_t bus, uint8_t device,uint8_t func, uint8_t register_offset);
void write_to_pci( uint32_t address);
int checkAHCI(uint8_t bus,uint8_t device);
void write_to_data();
uint32_t get_AHCIBASE(uint8_t bus,uint8_t device);
/*
typedef volatile struct tagHBA_PORT
{
	DWORD	clb;		// 0x00, command list base address, 1K-byte aligned
	DWORD	clbu;		// 0x04, command list base address upper 32 bits
	DWORD	fb;		// 0x08, FIS base address, 256-byte aligned
	DWORD	fbu;		// 0x0C, FIS base address upper 32 bits
	DWORD	is;		// 0x10, interrupt status
	DWORD	ie;		// 0x14, interrupt enable
	DWORD	cmd;		// 0x18, command and status
	DWORD	rsv0;		// 0x1C, Reserved
	DWORD	tfd;		// 0x20, task file data
	DWORD	sig;		// 0x24, signature
	DWORD	ssts;		// 0x28, SATA status (SCR0:SStatus)
	DWORD	sctl;		// 0x2C, SATA control (SCR2:SControl)
	DWORD	serr;		// 0x30, SATA error (SCR1:SError)
	DWORD	sact;		// 0x34, SATA active (SCR3:SActive)
	DWORD	ci;		// 0x38, command issue
	DWORD	sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	DWORD	fbs;		// 0x40, FIS-based switch control
	DWORD	rsv1[11];	// 0x44 ~ 0x6F, Reserved
	DWORD	vendor[4];	// 0x70 ~ 0x7F, vendor specific
} HBA_PORT;


#define HBA_PxCMD_CR            (1 << 15)  CR - Command list Running *//*
#define HBA_PxCMD_FR            (1 << 14)  FR - FIS receive Running 
#define HBA_PxCMD_FRE           (1 <<  4)  FRE - FIS Receive Enable
#define HBA_PxCMD_SUD           (1 <<  1)  SUD - Spin-Up Device 
#define HBA_PxCMD_ST            (1 <<  0)  ST - Start (command processing) */
/*typedef struct tagHBA_CMD_HEADER
{
	// DW0
	BYTE	cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
	BYTE	a:1;		// ATAPI
	BYTE	w:1;		// Write, 1: H2D, 0: D2H
	BYTE	p:1;		// Prefetchable
 
	BYTE	r:1;		// Reset
	BYTE	b:1;		// BIST
	BYTE	c:1;		// Clear busy upon R_OK
	BYTE	rsv0:1;		// Reserved
	BYTE	pmp:4;		// Port multiplier port
 
	WORD	prdtl;		// Physical region descriptor table length in entries
 
	// DW1
	volatile
	DWORD	prdbc;		// Physical region descriptor byte count transferred
 
	// DW2, 3
	DWORD	ctba;		// Command table descriptor base address
	DWORD	ctbau;		// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	DWORD	rsv1[4];	// Reserved
} HBA_CMD_HEADER;
*/

uint16_t checkDevice(uint8_t bus_slot, uint8_t device_slot);
