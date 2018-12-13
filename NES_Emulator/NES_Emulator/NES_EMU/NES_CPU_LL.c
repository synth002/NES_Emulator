

#include <stdio.h>
#include "NES_CPU.h"



/*
--- NES_CPU_LL (Low Level)
*/


//**** Shared globals ****
extern CPU_registers CPU_REGISTERS;
extern unsigned char NES_MEMORY[65536];
extern ROM_data		 ROM_DATA;
extern FILE *CPU_LOG;



unsigned char Memory_access(unsigned char operation, unsigned short memory_address, unsigned char data) {

	/*

	MAPPER BANK SWITCHES MUST BE HANDLED IN THIS FUNCTION!!!
	THE MAPPER FUNCTION POINTER WILL BE CALLED WHEN A BANK SWITCH IS DETECTED.

	------------>CPU MEMORY MAP<--------------

	Addr range			Size		Mapping
	0x0000 - 0x07FF		0x0800		2KB internal RAM
	0x0800 - 0x0FFF		0x0800		Mirrors 1 $0000 - $07FF
	0x1000 - 0x17FF		0x0800		Mirrors 2 $0000 - $07FF
	0x1800 - 0x1FFF		0x0800		Mirrors 3 $0000 - $07FF
	0x2000 - 0x2007		0x0008		NES PPU registers
	0x2008 - 0x3FFF		0x1FF8		Mirrors of $2000 - 2007 (repeats every 8 bytes)
	0x4000 - 0x4017		0x0018		NES APU and I / O registers
	0x4018 - 0x401F		0x0008		APU and I / O functionality that is normally disabled.See CPU Test Mode.
	0x4020 - 0xFFFF		0xBFE0		Cartridge space : PRG ROM, PRG RAM, and mapper registers.
	*/

	static unsigned short prev_fetch_addr = 0;
	if (operation == fetch_op) {
		prev_fetch_addr = memory_address;
	}
	else if (operation == fetch_previous) {
		memory_address = prev_fetch_addr;
	}

	unsigned char fetched_byte = 0;

	//Apply mapper mask
	memory_address &= ROM_DATA.mapper_mask;

	//Handle CPU memory mirroring
	if (memory_address < 0x2000) {
		memory_address &= ~(0x03 << 11);
	}

	//Mirror through 0x2000 - 0x2007 address range
	if (memory_address < 0x4000 && memory_address > 0x1FFF) {
		memory_address &= ~(0xFF << 3);
	}

	//Perform read/write operation
	if (operation == fetch_op) {
		return NES_MEMORY[memory_address];
	}
	else if (operation == write_op) {
		NES_MEMORY[memory_address] = data;
	}
	else if (operation == fetch_previous) {
		return NES_MEMORY[memory_address];
	}

	return 0;
}


bool Check_for_page_crossing(unsigned short value) {

	if (value > 0xFF) return true;
	return false;
}

