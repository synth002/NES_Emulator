

#include <stdio.h>
#include "NES_CPU.h"



/*
--- NES_CPU_LL (Low Level)
*/


//**** Extern globals ****
extern CPU_registers CPU_REGISTERS;
extern unsigned char NES_MEMORY[65536];
extern ROM_data		 ROM_DATA;



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

	unsigned char fetched_byte = 0;
	static unsigned short prev_fetch_addr = 0;

	if (operation == fetch_op) prev_fetch_addr = memory_address;
	else if (operation == fetch_previous) memory_address = prev_fetch_addr;

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

	//Perform regular read/write operation
	if (operation == fetch_op) return NES_MEMORY[memory_address];
	else if (operation == write_op) NES_MEMORY[memory_address] = data;
	else if (operation == fetch_previous) return NES_MEMORY[memory_address];

	return 0;
}




bool Check_for_page_crossing(unsigned short value) {

	if (value > 0xFF) return true;
	return false;
}




//************************************************************************************************
//************************************************************************************************
//********** CPU INSTRUCTION EXECUTION FUNCTIONS *************************************************
void ADC(cpu_emu_dat *cpu_emu_data) {

	//ADC - Add memory to accumulator with carry
	//NOTE: NES doesn't make use of decimal mode
	//Affects flags - N,Z,C,V 
	unsigned char sum;
	unsigned short temp;

	sum = CPU_REGISTERS.ACC_REG + cpu_emu_data->data + CPU_REGISTERS.S_REG.BIT.C;
	temp = CPU_REGISTERS.ACC_REG + cpu_emu_data->data + CPU_REGISTERS.S_REG.BIT.C;

	if ((CPU_REGISTERS.ACC_REG ^ sum) & (cpu_emu_data->data ^ sum) & 0x80) CPU_REGISTERS.S_REG.BIT.V = 1;
	else CPU_REGISTERS.S_REG.BIT.V = 0;


	CPU_REGISTERS.ACC_REG += cpu_emu_data->data + CPU_REGISTERS.S_REG.BIT.C;
	CPU_REGISTERS.S_REG.BIT.C = 0;

	if (temp > 0x00FF) CPU_REGISTERS.S_REG.BIT.C = 1;

	//Update negative flag
	if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void AND(cpu_emu_dat *cpu_emu_data) {

	//AND - 'AND' memory with accumulator
	//Affects flags - N,Z
	CPU_REGISTERS.ACC_REG &= cpu_emu_data->data;
	//Update negative flag
	if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void ASL(cpu_emu_dat *cpu_emu_data) {

	//ASL - Shift memory or accumulator left by one,
	//carry flag is set to value of bit 7 of operand.
	//Affects flags - N,Z,C
	if (cpu_emu_data->address_mode == accumulator) {
		CPU_REGISTERS.S_REG.BIT.C = (CPU_REGISTERS.ACC_REG & (1 << 7)) >> 7;
		CPU_REGISTERS.ACC_REG <<= 1;
		//Update negative flag
		if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
		else CPU_REGISTERS.S_REG.BIT.N = 0;
		//Update zero flag
		if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
		else CPU_REGISTERS.S_REG.BIT.Z = 0;
	}
	else {
		CPU_REGISTERS.S_REG.BIT.C = (cpu_emu_data->data & (1 << 7)) >> 7;
		cpu_emu_data->data <<= 1;
		//Update negative flag
		if (cpu_emu_data->data & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
		else CPU_REGISTERS.S_REG.BIT.N = 0;
		//Update zero flag
		if (cpu_emu_data->data == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
		else CPU_REGISTERS.S_REG.BIT.Z = 0;
	}
}


void BCC(cpu_emu_dat *cpu_emu_data) {

	//BCC - Branch on carry clear (if C = 0)
	//Affects flags - NONE
	if (CPU_REGISTERS.S_REG.BIT.C == 0) cpu_emu_data->branch_taken = true;
	else cpu_emu_data->branch_taken = false;
}


void BCS(cpu_emu_dat *cpu_emu_data) {

	//BCS - Branch on carry set (if C = 1)
	//Affects flags - NONE
	if (CPU_REGISTERS.S_REG.BIT.C == 1) cpu_emu_data->branch_taken = true;
	else cpu_emu_data->branch_taken = false;
}


void BEQ(cpu_emu_dat *cpu_emu_data) {

	//BEQ - Branch on result zero (if Z = 1)
	//Affects flags - NONE
	if (CPU_REGISTERS.S_REG.BIT.Z == 1) cpu_emu_data->branch_taken = true;
	else cpu_emu_data->branch_taken = false;
}


void BIT(cpu_emu_dat *cpu_emu_data) {

	//BIT - 'AND' memory with accumulator, if result 0, set flag Z = 1.
	//Also, values of memory bits M7 and M6 are copied to flags N,V
	//Neither accumulator nor memory data are altered!
	//Affects flags - N,Z,V
	unsigned char temp = (CPU_REGISTERS.ACC_REG & cpu_emu_data->data);
	CPU_REGISTERS.S_REG.BIT.N = (cpu_emu_data->data & (1 << 7)) >> 7;
	CPU_REGISTERS.S_REG.BIT.V = (cpu_emu_data->data & (1 << 6)) >> 6;
	//Update zero flag
	if (temp == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void BMI(cpu_emu_dat *cpu_emu_data) {

	//BMI - Branch on result minus (branch if flag N = 1)
	//Affects flags - NONE
	if (CPU_REGISTERS.S_REG.BIT.N == 1) cpu_emu_data->branch_taken = true;
	else cpu_emu_data->branch_taken = false;
}


void BNE(cpu_emu_dat *cpu_emu_data) {

	//BNE - Branch on result not zero (branch if flag Z = 0)
	//Affects flags - NONE
	if (CPU_REGISTERS.S_REG.BIT.Z == 0) cpu_emu_data->branch_taken = true;
	else cpu_emu_data->branch_taken = false;
}


void BPL(cpu_emu_dat *cpu_emu_data) {

	//BPL - Branch on result positive (branch if flag N = 0)
	//Affects flags - NONE
	if (CPU_REGISTERS.S_REG.BIT.N == 0) cpu_emu_data->branch_taken = true;
	else cpu_emu_data->branch_taken = false;
}


/*void BRK(cpu_emu_dat *cpu_emu_data) {

//This operation is handled entirely by the state machine
//FIND OUT IF I flag needs to be set!
//BRK - Force break. Increment progran counter by 2 then push to stack (push high byte first).
//Set flag B = 1, then push the status register to the stack.
//Load values 0xFFFE (LSB), and 0xFFFF (MSB) into program counter.
//Affects flags - B
}*/


void BVC(cpu_emu_dat *cpu_emu_data) {

	//BVC - Branch on overflow clear (branch if flag V = 0)
	//Affects flags - NONE
	if (CPU_REGISTERS.S_REG.BIT.V == 0) cpu_emu_data->branch_taken = true;
	else cpu_emu_data->branch_taken = false;
}


void BVS(cpu_emu_dat *cpu_emu_data) {

	//BVS - Branch on overflow set (branch if flag V = 1)
	//Affects flags - NONE
	if (CPU_REGISTERS.S_REG.BIT.V == 1) cpu_emu_data->branch_taken = true;
	else cpu_emu_data->branch_taken = false;
}


void CLC(cpu_emu_dat *cpu_emu_data) {

	//CLC - Clear carry flag
	//Affects flags - C
	CPU_REGISTERS.S_REG.BIT.C = 0;
}


void CLD(cpu_emu_dat *cpu_emu_data) {

	//CLD - Clear decimal flag
	//Affects flags - D
	CPU_REGISTERS.S_REG.BIT.D = 0;
}


void CLI(cpu_emu_dat *cpu_emu_data) {

	//CLI - Clear IRQ disable flag
	//Affects flags - I
	CPU_REGISTERS.S_REG.BIT.I = 0;
}


void CLV(cpu_emu_dat *cpu_emu_data) {

	//CLV - Clear overflow flag
	//Affects flags - V
	CPU_REGISTERS.S_REG.BIT.V = 0;
}


void CMP(cpu_emu_dat *cpu_emu_data) {

	//CMP - Subtract memory from accumulator to compare values. Result not kept.
	//Set flag C if A >= M, Set flag Z if A = M, Set flag N if bit 7 of result is set 
	//Affects flags - N,Z,C
	unsigned char result = CPU_REGISTERS.ACC_REG - cpu_emu_data->data;
	if (CPU_REGISTERS.ACC_REG >= cpu_emu_data->data) CPU_REGISTERS.S_REG.BIT.C = 1;
	else CPU_REGISTERS.S_REG.BIT.C = 0;
	if (CPU_REGISTERS.ACC_REG == cpu_emu_data->data) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
	//Update negative flag
	if (result & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
}


void CPX(cpu_emu_dat *cpu_emu_data) {

	//CMP - Subtract memory from X register to compare values. Result not kept.
	//Set flag C if X >= M, Set flag Z if X = M, Set flag N if bit 7 of result is set 
	//Affects flags - N,Z,C
	unsigned char result = CPU_REGISTERS.X_REG - cpu_emu_data->data;
	if (CPU_REGISTERS.X_REG >= cpu_emu_data->data) CPU_REGISTERS.S_REG.BIT.C = 1;
	else CPU_REGISTERS.S_REG.BIT.C = 0;
	if (CPU_REGISTERS.X_REG == cpu_emu_data->data) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
	//Update negative flag
	if (result & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
}


void CPY(cpu_emu_dat *cpu_emu_data) {

	//CMP - Subtract memory from Y register to compare values. Result not kept.
	//Set flag C if Y >= M, Set flag Z if Y = M, Set flag N if bit 7 of result is set 
	//Affects flags - N,Z,C
	unsigned char result = CPU_REGISTERS.Y_REG - cpu_emu_data->data;
	if (CPU_REGISTERS.Y_REG >= cpu_emu_data->data) CPU_REGISTERS.S_REG.BIT.C = 1;
	else CPU_REGISTERS.S_REG.BIT.C = 0;
	if (CPU_REGISTERS.Y_REG == cpu_emu_data->data) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
	//Update negative flag
	if (result & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
}


void DEC(cpu_emu_dat *cpu_emu_data) {

	//DEC - Decrement memory. Subtract one from the value held in memory (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	cpu_emu_data->data--;
	//Update negative flag
	if (cpu_emu_data->data & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (cpu_emu_data->data == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void DEX(cpu_emu_dat *cpu_emu_data) {

	//DEX - Decrement value in X register by one (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z, N
	CPU_REGISTERS.X_REG--;
	//Update negative flag
	if (CPU_REGISTERS.X_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.X_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void DEY(cpu_emu_dat *cpu_emu_data) {

	//DEX - Decrement value in Y register by one (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.Y_REG--;
	//Update negative flag
	if (CPU_REGISTERS.Y_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.Y_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void EOR(cpu_emu_dat *cpu_emu_data) {

	//EOR - Exclusive or memory and accumulator (result is kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.ACC_REG ^= cpu_emu_data->data;
	//Update negative flag
	if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void INC(cpu_emu_dat *cpu_emu_data) {

	//INC - Increment memory. Add one to the value stored in memory (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N 
	cpu_emu_data->data++;
	//Update negative flag
	if (cpu_emu_data->data & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (cpu_emu_data->data == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void INX(cpu_emu_dat *cpu_emu_data) {

	//DEX - Increment value in X register by one (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.X_REG++;
	//Update negative flag
	if (CPU_REGISTERS.X_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.X_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void INY(cpu_emu_dat *cpu_emu_data) {

	//DEY - Increment value in Y register by one (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.Y_REG++;
	//Update negative flag
	if (CPU_REGISTERS.Y_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.Y_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


/*void JMP(cpu_emu_dat *cpu_emu_data) {

//This operation is handled entirely by the state machine!
//JMP - Jump. Set the program counter to the value specified by the operand.
//Affects flags - NONE


//An original 6502 has does not correctly fetch the target address if the
//indirect vector falls on a page boundary(e.g.$xxFF where xx is any value
//from $00 to $FF). In this case fetches the LSB from $xxFF as expected but
//takes the MSB from $xx00. This is fixed in some later chips like the
//65SC02 so for compatibility always ensure the indirect vector is not at
//the end of the page.

}*/


/*void JSR(cpu_emu_dat *cpu_emu_data) {

//This operation is handled entirely by the state machine!
//JSR - Jump to subroutine. Address of the last byte of JSR (3 byte command) is pushed to stack.  (confirmed - see RTS opcode)
//The 2 byte address value specified by JSR is then loaded into the program counter.
//Affects flags - NONE
}*/


void LDA(cpu_emu_dat *cpu_emu_data) {

	//LDA - Load accumulator. Loads a byte of memory into the accumulator.
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.ACC_REG = cpu_emu_data->data;
	//Update negative flag
	if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void LDX(cpu_emu_dat *cpu_emu_data) {

	//LDX - Load X register. Loads a byte of memory into the X register.
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.X_REG = cpu_emu_data->data;
	//Update negative flag
	if (CPU_REGISTERS.X_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.X_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void LDY(cpu_emu_dat *cpu_emu_data) {

	//LDY - Load Y register. Loads a byte of memory into the Y register.
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.Y_REG = cpu_emu_data->data;
	//Update negative flag
	if (CPU_REGISTERS.Y_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.Y_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void LSR(cpu_emu_dat *cpu_emu_data) {

	//LSR - Logical shift right. The accumulator or memory location specified is shifted right by one.
	//The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero (N flag).
	//Affects flags - C,Z,N
	if (cpu_emu_data->address_mode == accumulator) {
		CPU_REGISTERS.S_REG.BIT.C = CPU_REGISTERS.ACC_REG & 0x01;
		CPU_REGISTERS.ACC_REG >>= 1;
		//Update zero flag
		if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
		else CPU_REGISTERS.S_REG.BIT.Z = 0;
	}
	else {
		CPU_REGISTERS.S_REG.BIT.C = cpu_emu_data->data & 0x01;
		cpu_emu_data->data >>= 1;
		//Update zero flag
		if (cpu_emu_data->data == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
		else CPU_REGISTERS.S_REG.BIT.Z = 0;
	}
	CPU_REGISTERS.S_REG.BIT.N = 0;
}


/*void NOP(cpu_emu_dat *cpu_emu_data) {

//This operation is handled entirely by the state machine!
//NOP - No operation (2 cycles). Program counter incremement by one to next instruction.
//Affects flags - NONE
}*/


void ORA(cpu_emu_dat *cpu_emu_data) {

	//ORA - 'OR' memory with accumulator. Result -> A.
	//Affects flags - Z,N
	CPU_REGISTERS.ACC_REG |= cpu_emu_data->data;
	//Update negative flag
	if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


/*void PHA(cpu_emu_dat *cpu_emu_data) {

//This operation is handled entirely by the state machine!
//PHA - Push accumulator onto the stack.
//Affects flags - NONE
}*/


/*void PHP(cpu_emu_dat *cpu_emu_data) {

//This operation is handled entirely by the state machine!
//PHP - Push status register onto the stack.
//Affects flags - NONE
}*/


void PLA(cpu_emu_dat *cpu_emu_data) {

	//This operation is handled entirely by the state machine!
	//PLA - Pull accumulator from the stack.
	//Affects flags - Z,N
	//Update negative flag
	if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


/*void PLP(cpu_emu_dat *cpu_emu_data) {

//This operation is handled entirely by the state machine!
//PLP - Pull status register from the stack.
//Affects flags - Pulled from stack
}*/


void ROL(cpu_emu_dat *cpu_emu_data) {

	//ROL - Rotate memory or accumulator one bit left.
	//Bit 0 is filled with the current value of the carry flag
	//whilst the old bit 7 becomes the new carry flag value.
	//Affects flags - Z,N,C
	unsigned char temp = CPU_REGISTERS.S_REG.BIT.C;

	if (cpu_emu_data->address_mode == accumulator) {
		CPU_REGISTERS.S_REG.BIT.C = (CPU_REGISTERS.ACC_REG & (1 << 7)) >> 7;
		CPU_REGISTERS.ACC_REG <<= 1;
		CPU_REGISTERS.ACC_REG |= temp;
		//Update negative flag
		if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
		else CPU_REGISTERS.S_REG.BIT.N = 0;
		//Update zero flag
		if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
		else CPU_REGISTERS.S_REG.BIT.Z = 0;
	}
	else {
		CPU_REGISTERS.S_REG.BIT.C = (cpu_emu_data->data & (1 << 7)) >> 7;
		cpu_emu_data->data <<= 1;
		cpu_emu_data->data |= temp;
		//Update negative flag
		if (cpu_emu_data->data & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
		else CPU_REGISTERS.S_REG.BIT.N = 0;
		//Update zero flag
		if (cpu_emu_data->data == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
		else CPU_REGISTERS.S_REG.BIT.Z = 0;
	}
}


void ROR(cpu_emu_dat *cpu_emu_data) {

	//ROR - Rotate memory or accumulator one bit right.
	//Bit 7 is filled with the current value of the carry flag
	//whilst the old bit 0 becomes the new carry flag value.
	//Affects flags - Z,N,C
	unsigned char temp = CPU_REGISTERS.S_REG.BIT.C;

	if (cpu_emu_data->address_mode == accumulator) {
		CPU_REGISTERS.S_REG.BIT.C = (CPU_REGISTERS.ACC_REG & 0x01);
		CPU_REGISTERS.ACC_REG >>= 1;
		CPU_REGISTERS.ACC_REG |= (temp << 7);
		//Update negative flag
		if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
		else CPU_REGISTERS.S_REG.BIT.N = 0;
		//Update zero flag
		if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
		else CPU_REGISTERS.S_REG.BIT.Z = 0;
	}
	else {
		CPU_REGISTERS.S_REG.BIT.C = (cpu_emu_data->data & 0x01);
		cpu_emu_data->data >>= 1;
		cpu_emu_data->data |= (temp << 7);
		//Update negative flag
		if (cpu_emu_data->data & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
		else CPU_REGISTERS.S_REG.BIT.N = 0;
		//Update zero flag
		if (cpu_emu_data->data == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
		else CPU_REGISTERS.S_REG.BIT.Z = 0;
	}
}


/*void RTI(cpu_emu_dat *cpu_emu_data) {

//This operation is handled entirely by the state machine!
//RTI - Return from interrupt.
//Pull status register from stack, followed by program counter.
}*/


void RTS(cpu_emu_dat *cpu_emu_data) {

	//RTS - Return from subroutine.
	//Pull program counter from stack and add one to it 
	//This operation is handled mostly by the state machine!
	CPU_REGISTERS.PC.REG++;
}


void SBC(cpu_emu_dat *cpu_emu_data) {

	//SBC - Subtract with carry. The Carry flag in status reg will always
	//be set before this instruction is called. If the carry flag bit
	//is borrow from, it's value is 256 (and it will be cleared).
	//Affects flags - Z,N,C,V
	cpu_emu_data->data ^= 0xFF;	//Ones complicment
	ADC(cpu_emu_data);
}


void SEC(cpu_emu_dat *cpu_emu_data) {

	//SEC - Set carry flag
	//Affects flags - C
	CPU_REGISTERS.S_REG.BIT.C = 1;
}


void SED(cpu_emu_dat *cpu_emu_data) {

	//SED - Set decimal flag
	//Affects flags - D
	CPU_REGISTERS.S_REG.BIT.D = 1;
}


void SEI(cpu_emu_dat *cpu_emu_data) {

	//SEI - Set IRQ disable flag
	//Affects flags - I
	CPU_REGISTERS.S_REG.BIT.I = 1;
}


void STA(cpu_emu_dat *cpu_emu_data) {

	//STA - Stores value of accumulator to memory
	//Affects flags - NONE
	cpu_emu_data->data = CPU_REGISTERS.ACC_REG;
}


void STX(cpu_emu_dat *cpu_emu_data) {

	//STX - Stores value of X register to memory
	//Affects flags - NONE
	cpu_emu_data->data = CPU_REGISTERS.X_REG;
}


void STY(cpu_emu_dat *cpu_emu_data) {

	//STY - Stores value of Y register to memory
	//Affects flags - NONE
	cpu_emu_data->data = CPU_REGISTERS.Y_REG;
}


void TAX(cpu_emu_dat *cpu_emu_data) {

	//TAX - Transfer accumulator to X register
	//Affects flags - Z,N
	CPU_REGISTERS.X_REG = CPU_REGISTERS.ACC_REG;
	//Update negative flag
	if (CPU_REGISTERS.X_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.X_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void TAY(cpu_emu_dat *cpu_emu_data) {

	//TAY - Transfer accumulator to Y register
	//Affects flags - Z,N
	CPU_REGISTERS.Y_REG = CPU_REGISTERS.ACC_REG;
	//Update negative flag
	if (CPU_REGISTERS.Y_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.Y_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void TSX(cpu_emu_dat *cpu_emu_data) {

	//TSX - Transfer stack pointer to X register
	//Affects flags - Z,N
	CPU_REGISTERS.X_REG = CPU_REGISTERS.SP;
	//Update negative flag
	if (CPU_REGISTERS.X_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.X_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void TXA(cpu_emu_dat *cpu_emu_data) {

	//TXA - Tansfer X register to accumulator
	//Affects flags - Z,N
	CPU_REGISTERS.ACC_REG = CPU_REGISTERS.X_REG;
	//Update negative flag
	if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}


void TXS(cpu_emu_dat *cpu_emu_data) {

	//TXS - Transfer X register to stack pointer
	//Affects flags - NONE
	CPU_REGISTERS.SP = CPU_REGISTERS.X_REG;
}


void TYA(cpu_emu_dat *cpu_emu_data) {

	//TYA - Transfer Y register into the accumulator
	//Affects flags - Z,N
	CPU_REGISTERS.ACC_REG = CPU_REGISTERS.Y_REG;
	//Update negative flag
	if (CPU_REGISTERS.ACC_REG & (1 << 7)) CPU_REGISTERS.S_REG.BIT.N = 1;
	else CPU_REGISTERS.S_REG.BIT.N = 0;
	//Update zero flag
	if (CPU_REGISTERS.ACC_REG == 0) CPU_REGISTERS.S_REG.BIT.Z = 1;
	else CPU_REGISTERS.S_REG.BIT.Z = 0;
}
//************************************************************************************************
//************************************************************************************************
//************************************************************************************************
