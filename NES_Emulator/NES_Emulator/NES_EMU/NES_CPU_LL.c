
#include "NES_CPU.h"


/*
--- NES_CPU_LL (Low Level)
*/


//**** Shared globals ****
extern CPU_registers CPU_REGISTERS;
extern unsigned char NES_MEMORY[65536];
extern ROM_data		 ROM_DATA;




unsigned char Memory_access(unsigned char rw, unsigned short memory_address, unsigned char data) {

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

	unsigned char fetched_byte = 0;

	//Apply mapper mask
	memory_address &= ROM_DATA.mapper_mask;

	//Lines 11, 12 of CPU address bus are always tied low
	memory_address &= ~(0x03 << 11);

	//Mirror through 0x2000 - 0x2007 address range
	if ((memory_address & (0x07 << 13)) == 0x01) {
		memory_address &= ~(0xFF << 3);
	}

	//Perform read/write operation
	if (rw == fetch_op) {
		return NES_MEMORY[memory_address];
	}
	else {
		NES_MEMORY[memory_address] = data;
	}

	return 0;
}


bool Check_for_page_crossing(unsigned short value) {

	if (value > 0xFF) return true;
	return false;
}


void Update_overflow_flag(unsigned char acc, unsigned char val) {

	if(((signed char)acc < 0) && ((signed char)val < 0)) {
	
	}
}


void Update_negative_flag(unsigned char val) {

	if (val && (1 << 7)) {
		Set_negative_flag();
	}
	else {
		Clear_negative_flag();
	}
}



void Update_zero_flag(unsigned char val) {

	if (val == 0) {
		Set_zero_flag();
	}
	else {
		Clear_zero_flag();
	}
}



//******************************************************
//****Functions for setting CPU status register bits****
void Set_negative_flag(void) {

	//This function sets the negative flag in CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.N = 1;
}

void Set_zero_flag(void) {

	//This function sets the zero flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.Z = 1;
}

void Set_carry_flag(void) {

	//This function sets the carry flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.C = 1;
}

void Set_interrupt_flag(void) {

	//This function sets the IRQ disable flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.I = 1;
}

void Set_decimal_flag(void) {

	//This function sets the decimal mode flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.D = 1;
}

void Set_break_flag(void) {

	//This function sets the break flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.B = 1;
}

void Set_overflow_flag(void) {

	//This function sets the overflow flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.V = 1;
}
//******************************************************
//******************************************************




//******************************************************
//****Functions for setting CPU status register bits****
void Clear_negative_flag(void) {

	//This function sets the negative flag in CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.N = 0;
}

void Clear_zero_flag(void) {

	//This function sets the zero flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.Z = 0;
}

void Clear_carry_flag(void) {

	//This function sets the carry flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.C = 0;
}

void Clear_interrupt_flag(void) {

	//This function sets the IRQ disable flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.I = 0;
}

void Clear_decimal_flag(void) {

	//This function sets the decimal mode flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.D = 0;
}

void Clear_break_flag(void) {

	//This function sets the break flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.B = 0;
}

void Clear_overflow_flag(void) {

	//This function sets the overflow flag in the CPU status register
	CPU_REGISTERS.CPU_STATUS_REG.BIT.V = 0;
}
//******************************************************
//******************************************************




//****************************************************************
//******Functions to check value of CPU status register bits******
unsigned char Check_negative_flag(void) {

	//This function returns the value of the 
	//negative flag in CPU status register
	return CPU_REGISTERS.CPU_STATUS_REG.BIT.N;
}

unsigned char Check_zero_flag(void) {

	//This function returns the value of the 
	//zero flag in the CPU status register
	return CPU_REGISTERS.CPU_STATUS_REG.BIT.Z;
}

unsigned char Check_carry_flag(void) {

	//This function returns the value of the 
	//carry flag in the CPU status register
	return CPU_REGISTERS.CPU_STATUS_REG.BIT.C;
}

unsigned char Check_interrupt_flag(void) {

	//This funtion returns the value of the 
	//interrupt disable flag in the CPU status register
	return CPU_REGISTERS.CPU_STATUS_REG.BIT.I;
}

unsigned char Check_decimal_flag(void) {

	//This function returns the value of the 
	//decimal mode flag in the CPU status register
	return CPU_REGISTERS.CPU_STATUS_REG.BIT.D;
}

unsigned char Check_break_flag(void) {

	//This function returns the value of the 
	//break command flag in the CPU status register
	return CPU_REGISTERS.CPU_STATUS_REG.BIT.B;
}

unsigned char Check_overflow_flag(void) {

	//This function returns the value of the
	//overflow flag in the CPU status register
	return CPU_REGISTERS.CPU_STATUS_REG.BIT.V;
}
//****************************************************************
//****************************************************************