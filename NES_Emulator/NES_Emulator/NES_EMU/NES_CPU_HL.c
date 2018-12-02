#include "NES_CPU_Shared.h"
#include "NES_CPU.h"


/*
	---> NES_CPU_HL (High Level)
*/


//Single byte operations
unsigned char single_byte_intstruction[3]	 =  { 0, 3, 0xFF };
//Read operations
unsigned char read_execute_immediate[3]      =  { 0, 3, 0xFF };
unsigned char read_execute_zeropage[4]       =  { 0, 4, 7, 0xFF };
unsigned char read_execute_absolute[5]       =  { 0, 4, 5, 8, 0xFF };
unsigned char read_execute_indirect_x[7]     =  { 0, 4, 1, 9, 12, 13, 0xFF };
unsigned char read_execute_absolute_x[6]     =  { 0, 4, 5, 18, 18, 0xFF };			//Skip second 18 state if page crossed = false!
unsigned char read_execute_absolute_y[6]     =  { 0, 4, 5, 19, 19, 0xFF };			//Skip second 19 state if page crossed = false!
unsigned char read_execute_zeropage_x[5]     =  { 0, 4, 1, 14, 0xFF };
unsigned char read_execute_zeropage_y[5]     =  { 0, 4, 1, 15, 0xFF };
unsigned char read_execute_indirect_y[7]     =  { 0, 4, 11, 12, 20, 20, 0xFF };		//Skip second 20 state if page crossed = false!
//Store operations
unsigned char store_operation_zeropage[4]    =  { 0, 4, 34, 0xFF };
unsigned char store_operation_absolute[5]    =  { 0, 4, 5, 34, 0xFF };
unsigned char store_operation_indirect_x[7]  =  { 0, 4, 1, 9, 12, 39, 0xFF };
unsigned char store_operation_absolute_x[6]  =  { 0, 4, 5, 16, 39, 0xFF };
unsigned char store_operation_absolute_y[6]  =  { 0, 4, 5, 17, 39, 0xFF };
unsigned char store_operation_zeropage_x[6]  =  { 0, 4, 1, 36, 0xFF };
unsigned char store_operation_zeropage_y[6]  =  { 0, 4, 1, 37, 0xFF };
unsigned char store_operation_indirect_y[7]  =  { 0, 4, 11, 12, 21, 39, 0xFF };
//RMW operations
unsigned char modify_write_zeropage[6]		 =  { 0, 4, 7, 35, 35, 0xFF };
unsigned char modify_write_absolute[7]		 =  { 0, 4, 5, 8, 33, 33, 0xFF };
unsigned char modify_write_zeropage_x[7]	 =  { 0, 4, 1, 14, 35, 35, 0xFF };
unsigned char modify_write_absolute_x[8]	 =  { 0, 4, 5, 16, 13, 38, 38, 0xFF };
//Stack push operations
unsigned char push_operation_php[4]			 = { 0, 2, 26, 0xFF };
unsigned char push_operation_pha[4]			 = { 0, 2, 27, 0xFF };
//Stack pull operations
unsigned char pull_operation_plp[5]			 = { 0, 2, 28, 30, 0xFF };
unsigned char pull_operation_pla[5]			 = { 0, 2, 28, 29, 0xFF };
//Subroutine operations
unsigned char jump_to_subroutine[7]			 = { 0, 4, 28, 24, 25, 6, 0xFF };
unsigned char rtn_from_subroutine[7]		 = { 0, 2, 28, 31, 32, 2, 0xFF };
//Break/interrupt operations
unsigned char break_operation_irq[8]		 = { 0, 2, 24, 25, 26, 41, 42, 0xFF };
unsigned char rtn_from_interrupt[7]			 = { 0, 2, 28, 30, 31, 32, 0xFF };
//Jump operations
unsigned char jump_operation_absolute[4]	 = { 0, 4, 6, 0xFF };
unsigned char jump_operation_indirect[6]	 = { 0, 4, 5, 22, 23, 0xFF };
//Branch operations
unsigned char branch_operation[5]			 = { 0, 3, 40, 40, 0xFF};				//Only continute after 2nd state if branch_taken = true. Only continue after 3rd state if page_crossed = true. 
//NOP operation 
unsigned char nop_operation[3]				 = { 0, 2, 0xFF };




//**** Local globals ****
CPU_registers	CPU_REGISTERS;


//**** Shared globals ****
extern unsigned char	NES_MEMORY[65536];




//******************************************************
//************* Setup CPU ******************************
void Setup_CPU(void) {

	CPU_REGISTERS.X_REG = 0;
	CPU_REGISTERS.Y_REG = 0;
	CPU_REGISTERS.ACC_REG = 0;
	CPU_REGISTERS.RESET = 1;
	CPU_REGISTERS.STACK_POINTER = 0xFD;
	CPU_REGISTERS.CPU_STATUS_REG.REG = 0x34;
	CPU_REGISTERS.PROGRAM_COUNTER.BYTE.LOW = 0x00;
	CPU_REGISTERS.PROGRAM_COUNTER.BYTE.HIGH = 0xC0;
}
//******************************************************
//******************************************************




//******************************************************
//********** Init CPU runtime attributes ***************
void Init_attributes(cpu_emu_dat *cpu_emu_data) {

	CPU_REGISTERS.RESET = 0;
	cpu_emu_data->cycle = 0;
	cpu_emu_data->opcode = 0;
	cpu_emu_data->irq = no_irq;
	cpu_emu_data->address_mode = implied;
	cpu_emu_data->data = 0;
	cpu_emu_data->base_addr.reg = 0;
	cpu_emu_data->page_crossed = none;
	cpu_emu_data->branch_taken = true;
	cpu_emu_data->indexed_addr.reg = 0;
	cpu_emu_data->state = 0;
}
//******************************************************
//******************************************************




//****************************************************************
//*************** NES CPU STATE MACHINE **************************
void CPU_cycle(void) {

	static cpu_emu_dat cpu_emu_data;

	printf("stuff happening!");


	if (CPU_REGISTERS.RESET > 0) {
		Init_attributes(&cpu_emu_data);
	}


	if (cpu_emu_data.cycle == 0) {
		cpu_emu_data.data = 0;
		cpu_emu_data.base_addr.reg = 0;
		cpu_emu_data.page_crossed = none;
		cpu_emu_data.branch_taken = true;
		cpu_emu_data.indexed_addr.reg = 0;
		if( (cpu_emu_data.address_mode != implied) && (cpu_emu_data.address_mode != accumulator) ) {
			CPU_REGISTERS.PROGRAM_COUNTER.REG++;
		}
		cpu_emu_data.opcode = Memory_access(fetch_op, CPU_REGISTERS.PROGRAM_COUNTER.REG, 0);
		Instruction_lookup(&cpu_emu_data);
	}


	switch (cpu_emu_data.state[cpu_emu_data.cycle]) {


	case 1 :	//Data read (data not used) - Base_addr LOW
		cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.base_addr.byte.low, 0);
		break;


	case 2 :	//Data read (data not used) - Program counter
		CPU_REGISTERS.PROGRAM_COUNTER.REG++;
		cpu_emu_data.data = Memory_access(fetch_op, CPU_REGISTERS.PROGRAM_COUNTER.REG, 0);
		break;


	case 3 :	//Data read & execute - Addr = program counter
		CPU_REGISTERS.PROGRAM_COUNTER.REG++;
		cpu_emu_data.data = Memory_access(fetch_op, CPU_REGISTERS.PROGRAM_COUNTER.REG, 0);
		cpu_emu_data.instruction_ptr(&cpu_emu_data);
		break;


	case 4 :	//Fetch - base_addr LSB
		CPU_REGISTERS.PROGRAM_COUNTER.REG++;
		cpu_emu_data.base_addr.byte.low = Memory_access(fetch_op, CPU_REGISTERS.PROGRAM_COUNTER.REG, 0);
		break;


	case 5:		//Fetch - base_addr MSB
		CPU_REGISTERS.PROGRAM_COUNTER.REG++;
		cpu_emu_data.base_addr.byte.high = Memory_access(fetch_op, CPU_REGISTERS.PROGRAM_COUNTER.REG, 0);
		break;


	case 6 :	//Fetch - Base_addr MSB - (Copy Base_addr to Program_counter - JSR, JMP)
		CPU_REGISTERS.PROGRAM_COUNTER.REG++;
		cpu_emu_data.base_addr.byte.high = Memory_access(fetch_op, CPU_REGISTERS.PROGRAM_COUNTER.REG, 0);
		CPU_REGISTERS.PROGRAM_COUNTER.REG = cpu_emu_data.base_addr.reg;
		if (cpu_emu_data.address_mode == absolute) {
			CPU_REGISTERS.PROGRAM_COUNTER.REG--;
		}
		break;


	case 7 :	//Data read & execute - Base_addr LSB 
		cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.base_addr.byte.low, 0);
		cpu_emu_data.instruction_ptr(&cpu_emu_data);
		break;


	case 8 :	//Data read & execute - Base_addr FULL
		cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.base_addr.reg, 0);
		cpu_emu_data.instruction_ptr(&cpu_emu_data);
		break;


	case 9 :	//Fetch indexed_addr LSB (indexed by X_REG)
		cpu_emu_data.base_addr.byte.low += CPU_REGISTERS.X_REG;
		cpu_emu_data.indexed_addr.byte.low = Memory_access(fetch_op, cpu_emu_data.base_addr.byte.low, 0);
		break;


	case 10:	//Fetch indexed_addr LSB (indexed by Y_REG)
		cpu_emu_data.base_addr.byte.low += CPU_REGISTERS.Y_REG;
		cpu_emu_data.indexed_addr.byte.low = Memory_access(fetch_op, cpu_emu_data.base_addr.byte.low, 0);
		break;


	case 11 :	//Fetch indexed_addr LSB (Base_addr LOW)
		cpu_emu_data.indexed_addr.byte.low = Memory_access(fetch_op, cpu_emu_data.base_addr.byte.low, 0);
		break;


	case 12 :	//Fetch indexed_addr MSB (Base_addr LOW + 1)
		cpu_emu_data.base_addr.byte.low += 1;
		cpu_emu_data.indexed_addr.byte.high = Memory_access(fetch_op, cpu_emu_data.base_addr.byte.low, 0);
		break;


	case 13 :	//Data read & execute - indexed_addr FULL
		cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.indexed_addr.reg, 0);
		cpu_emu_data.instruction_ptr(&cpu_emu_data);
		break;


	case 14 :	//Data read & execute - Base_addr LSB + Zero page X
		cpu_emu_data.base_addr.byte.low += CPU_REGISTERS.X_REG;
		cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.base_addr.byte.low, 0);
		cpu_emu_data.instruction_ptr(&cpu_emu_data);
		break;


	case 15 :	//Data read & execute - Base_addr LSB + Zero page Y
		cpu_emu_data.base_addr.byte.low += CPU_REGISTERS.Y_REG;
		cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.base_addr.byte.low, 0);
		cpu_emu_data.instruction_ptr(&cpu_emu_data);
		break;


	case 16 :	//For store ops - abs index X ******
		cpu_emu_data.page_crossed = Check_for_page_crossing(cpu_emu_data.base_addr.byte.low + CPU_REGISTERS.X_REG);
		cpu_emu_data.indexed_addr.byte.low = cpu_emu_data.base_addr.byte.low + CPU_REGISTERS.X_REG;
		cpu_emu_data.indexed_addr.byte.high = cpu_emu_data.base_addr.byte.high;
		cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.indexed_addr.reg, 0);
		if (cpu_emu_data.page_crossed == true) {
			cpu_emu_data.indexed_addr.byte.high += 1;
		}


	case 17 :	//For store ops - abs index Y *****
		cpu_emu_data.page_crossed = Check_for_page_crossing(cpu_emu_data.base_addr.byte.low + CPU_REGISTERS.Y_REG);
		cpu_emu_data.indexed_addr.byte.low = cpu_emu_data.base_addr.byte.low + CPU_REGISTERS.Y_REG;
		cpu_emu_data.indexed_addr.byte.high = cpu_emu_data.base_addr.byte.high;
		cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.indexed_addr.reg, 0);
		if (cpu_emu_data.page_crossed == true) {
			cpu_emu_data.indexed_addr.byte.high += 1;
		}


	case 18 :	//Data read (with page crossing detection) - Absolute (indexed by X_REG)
		if (cpu_emu_data.page_crossed == none) {
			cpu_emu_data.page_crossed = Check_for_page_crossing(cpu_emu_data.base_addr.byte.low + CPU_REGISTERS.X_REG);
			cpu_emu_data.indexed_addr.byte.low = cpu_emu_data.base_addr.byte.low + CPU_REGISTERS.X_REG;
			cpu_emu_data.indexed_addr.byte.high = cpu_emu_data.base_addr.byte.high;
			cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.indexed_addr.reg, 0);
			if (cpu_emu_data.page_crossed == false) {
				cpu_emu_data.instruction_ptr(&cpu_emu_data);
			}
		}
		else if (cpu_emu_data.page_crossed == true) {
			cpu_emu_data.indexed_addr.byte.high += 1;
			cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.indexed_addr.reg, 0);
			cpu_emu_data.instruction_ptr(&cpu_emu_data);
		}


	case 19:	//Data read (with page crossing detection) - Absolute (indexed by Y_REG)
		if (cpu_emu_data.page_crossed == none) {
			cpu_emu_data.page_crossed = Check_for_page_crossing(cpu_emu_data.base_addr.byte.low + CPU_REGISTERS.Y_REG);
			cpu_emu_data.indexed_addr.byte.low = cpu_emu_data.base_addr.byte.low + CPU_REGISTERS.Y_REG;
			cpu_emu_data.indexed_addr.byte.high = cpu_emu_data.base_addr.byte.high;
			cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.indexed_addr.reg, 0);
			if (cpu_emu_data.page_crossed == false) {
				cpu_emu_data.instruction_ptr(&cpu_emu_data);
			}
		}
		else if (cpu_emu_data.page_crossed == true) {
			cpu_emu_data.indexed_addr.byte.high += 1;
			cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.indexed_addr.reg, 0);
			cpu_emu_data.instruction_ptr(&cpu_emu_data);
		}
		break;


	case 20 :	//Data read (with page crossing detection) - Indirect  (indexed by Y_REG)
		if (cpu_emu_data.page_crossed == none) {
			cpu_emu_data.page_crossed = Check_for_page_crossing(cpu_emu_data.indexed_addr.byte.low + CPU_REGISTERS.Y_REG);
			cpu_emu_data.indexed_addr.byte.low += CPU_REGISTERS.Y_REG;
			cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.indexed_addr.reg, 0);
			if (cpu_emu_data.page_crossed == false) {
				cpu_emu_data.instruction_ptr(&cpu_emu_data);
			}
		}
		else  if (cpu_emu_data.page_crossed == true) {
			cpu_emu_data.indexed_addr.byte.high += 1;
			cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.indexed_addr.reg, 0);
			cpu_emu_data.instruction_ptr(&cpu_emu_data);
		}
		break;


	case 21:	//Data read (with page crossing detection) - FOR WRITES - Indirect  (indexed by Y_REG)
		cpu_emu_data.page_crossed = Check_for_page_crossing(cpu_emu_data.indexed_addr.byte.low + CPU_REGISTERS.Y_REG);
		cpu_emu_data.indexed_addr.byte.low += CPU_REGISTERS.Y_REG;
		cpu_emu_data.data = Memory_access(fetch_op, cpu_emu_data.indexed_addr.reg, 0);
		if (cpu_emu_data.page_crossed == true) {
			cpu_emu_data.indexed_addr.byte.high += 1;
		}
		break;


	case 22 :	//Absolute indirect (for JMP)
		CPU_REGISTERS.PROGRAM_COUNTER.BYTE.LOW = Memory_access(fetch_op, cpu_emu_data.base_addr.reg, 0);
		break;


	case 23:	//Absolute indirect + 1 (for JMP)
		cpu_emu_data.base_addr.byte.low++;
		CPU_REGISTERS.PROGRAM_COUNTER.BYTE.HIGH = Memory_access(fetch_op, cpu_emu_data.base_addr.reg, 0);
		CPU_REGISTERS.PROGRAM_COUNTER.REG--;
		break;

		//******************************** Stack pointer operations ********************************

	case 24 :	//Push PC.HIGH byte to stack
		Memory_access(write_op, CPU_REGISTERS.STACK_POINTER, CPU_REGISTERS.PROGRAM_COUNTER.BYTE.HIGH);
		CPU_REGISTERS.STACK_POINTER--;
		break;


	case 25 :	//Push PC.LOW byte to stack
		Memory_access(write_op, CPU_REGISTERS.STACK_POINTER, CPU_REGISTERS.PROGRAM_COUNTER.BYTE.LOW);
		CPU_REGISTERS.STACK_POINTER--;
		break;


	case 26 :	//Push CPU_STATUS_REG to stack
		Memory_access(write_op, CPU_REGISTERS.STACK_POINTER, CPU_REGISTERS.CPU_STATUS_REG.REG);					//*******************if BRK - SET B bit in status reg!*******************
		CPU_REGISTERS.STACK_POINTER--;
		break;


	case 27 :	//Push ACCUMULATOR to stack
		Memory_access(write_op, CPU_REGISTERS.STACK_POINTER, CPU_REGISTERS.ACC_REG);
		CPU_REGISTERS.STACK_POINTER--;
		break;


	case 28:	//Pull DATA from stack (data not used)
		Memory_access(fetch_op, CPU_REGISTERS.STACK_POINTER, 0);
		break;


	case 29:	//Pull ACCUMULATOR from stack
		CPU_REGISTERS.STACK_POINTER++;
		CPU_REGISTERS.ACC_REG = Memory_access(fetch_op, CPU_REGISTERS.STACK_POINTER, 0);
		break;


	case 30:	//Pull CPU_STATUS_REG from stack
		CPU_REGISTERS.STACK_POINTER++;
		CPU_REGISTERS.CPU_STATUS_REG.REG = Memory_access(fetch_op, CPU_REGISTERS.STACK_POINTER, 0);				//*******************if BRK - SET B bit in status reg!*******************
		break;


	case 31 :	//Pull PC.LOW byte from stack
		CPU_REGISTERS.STACK_POINTER++;
		CPU_REGISTERS.PROGRAM_COUNTER.BYTE.LOW = Memory_access(fetch_op, CPU_REGISTERS.STACK_POINTER, 0);
		break;


	case 32 :	//Pull PC.HIGH byte from stack
		CPU_REGISTERS.STACK_POINTER++;
		CPU_REGISTERS.PROGRAM_COUNTER.BYTE.HIGH = Memory_access(fetch_op, CPU_REGISTERS.STACK_POINTER, 0);
		break;

		//************************* WRITE OPS ***********************************

	case 33 :	//Write operation - Base addr FULL
		Memory_access(write_op, cpu_emu_data.base_addr.reg, cpu_emu_data.data);
		break;


	case 34:	//STORE - Base addr FULL (STA, STX, STY)
		cpu_emu_data.instruction_ptr(&cpu_emu_data);
		Memory_access(write_op, cpu_emu_data.base_addr.reg, cpu_emu_data.data);
		break;


	case 35 :	//Write operation - Base addr LOW
		Memory_access(write_op, cpu_emu_data.base_addr.byte.low, cpu_emu_data.data);
		break;


	case 36:	//STORE - Base addr LOW + X (STA, STX, STY)
		cpu_emu_data.instruction_ptr(&cpu_emu_data);
		cpu_emu_data.base_addr.byte.low += CPU_REGISTERS.X_REG;
		Memory_access(write_op, cpu_emu_data.base_addr.byte.low, cpu_emu_data.data);
		break;


	case 37:	//STORE - Base addr LOW + Y (STA, STX, STY)
		cpu_emu_data.instruction_ptr(&cpu_emu_data);
		cpu_emu_data.base_addr.byte.low += CPU_REGISTERS.Y_REG;
		Memory_access(write_op, cpu_emu_data.base_addr.byte.low, cpu_emu_data.data);
		break;


	case 38:	//Write operation - Indexed addr FULL
		Memory_access(write_op, cpu_emu_data.indexed_addr.reg, cpu_emu_data.data);
		break;


	case 39:	//STORE - Indexed addr FULL (STA, STX, STY)
		cpu_emu_data.instruction_ptr(&cpu_emu_data);
		Memory_access(write_op, cpu_emu_data.indexed_addr.reg, cpu_emu_data.data);
		break;

		//******************* Handle branching **************************

	case 40 :	
		if (cpu_emu_data.page_crossed == none) {
			CPU_REGISTERS.PROGRAM_COUNTER.REG++;
			cpu_emu_data.page_crossed = Check_for_page_crossing(CPU_REGISTERS.PROGRAM_COUNTER.BYTE.LOW + cpu_emu_data.data);
			CPU_REGISTERS.PROGRAM_COUNTER.BYTE.LOW += cpu_emu_data.data;
		}
		else if (cpu_emu_data.page_crossed == true) {
			CPU_REGISTERS.PROGRAM_COUNTER.BYTE.HIGH += 1;
		}

		//********************* Handle IRQ ******************************]

	case 41 :
		CPU_REGISTERS.PROGRAM_COUNTER.BYTE.LOW = Memory_access(fetch_op, 0xFFFE , 0);			//TODO: (BRK) detect different IRQ and corresponding vector!
		break;


	case 42 :
		CPU_REGISTERS.PROGRAM_COUNTER.BYTE.HIGH = Memory_access(fetch_op, 0xFFFF, 0);			//TODO: (BRK) detect different IRQ and corresponding vector!
		break;


	default:
		//Error 
		break;
	}
	

	//Increment cycle
	cpu_emu_data.cycle++;

	if (cpu_emu_data.page_crossed == false)  {
		if (cpu_emu_data.state[cpu_emu_data.cycle - 1] == cpu_emu_data.state[cpu_emu_data.cycle]) {
			cpu_emu_data.cycle++;
		}
	}

	if (cpu_emu_data.state[cpu_emu_data.cycle] == 0xFF || cpu_emu_data.branch_taken == false) {
		cpu_emu_data.cycle = 0;
	}
}
//****************************************************************
//****************************************************************




//********************************************************************************
//************************ DECODE OPCODE FUNCTION ********************************
void Instruction_lookup(cpu_emu_dat *cpu_emu_data) {

	//This function decodes the latest instruction opcode fetched,
	//it identifies the instruction - as well as the addressing mode
	//used. It updateS the 'cpu_emu_data' struct with the  instruction 
	//function pointer, as well as the addressing mode to be used.

	switch (cpu_emu_data->opcode) {

	case 0x6D:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &ADC;
		break;

	case 0x7D:
		cpu_emu_data->state = read_execute_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &ADC;
		break;

	case 0x79:
		cpu_emu_data->state = read_execute_absolute_y;
		cpu_emu_data->address_mode = absolute_y;
		cpu_emu_data->instruction_ptr = &ADC;
		break;

	case 0x69:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &ADC;
		break;

	case 0x61:
		cpu_emu_data->state = read_execute_indirect_x;
		cpu_emu_data->address_mode = indexed_indirect_x;
		cpu_emu_data->instruction_ptr = &ADC;
		break;

	case 0x71:
		cpu_emu_data->state = read_execute_indirect_y;
		cpu_emu_data->address_mode = indirect_indexed_y;
		cpu_emu_data->instruction_ptr = &ADC;
		break;

	case 0x65:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &ADC;
		break;

	case 0x75:
		cpu_emu_data->state = read_execute_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &ADC;
		break;

	case 0x2D:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &AND;
		break;

	case 0x3D:
		cpu_emu_data->state = read_execute_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &AND;
		break;

	case 0x39:
		cpu_emu_data->state = read_execute_absolute_y;
		cpu_emu_data->address_mode = absolute_y;
		cpu_emu_data->instruction_ptr = &AND;
		break;

	case 0x29:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &AND;
		break;

	case 0x21:
		cpu_emu_data->state = read_execute_indirect_x;
		cpu_emu_data->address_mode = indexed_indirect_x;
		cpu_emu_data->instruction_ptr = &AND;
		break;

	case 0x31:
		cpu_emu_data->state = read_execute_indirect_y;
		cpu_emu_data->address_mode = indirect_indexed_y;
		cpu_emu_data->instruction_ptr = &AND;
		break;

	case 0x25:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &AND;
		break;

	case 0x35:
		cpu_emu_data->state = read_execute_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &AND;
		break;

	case 0x0E:
		cpu_emu_data->state = modify_write_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &ASL;
		break;

	case 0x1E:
		cpu_emu_data->state = modify_write_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &ASL;
		break;

	case 0x0A:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = accumulator;
		cpu_emu_data->instruction_ptr = &ASL;
		break;

	case 0x06:
		cpu_emu_data->state = modify_write_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &ASL;
		break;

	case 0x16:
		cpu_emu_data->state = modify_write_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &ASL;
		break;

	case 0x90:
		cpu_emu_data->state = branch_operation;
		cpu_emu_data->address_mode = relative;
		cpu_emu_data->instruction_ptr = &BCC;
		break;

	case 0xB0:
		cpu_emu_data->state = branch_operation;
		cpu_emu_data->address_mode = relative;
		cpu_emu_data->instruction_ptr = &BCS;
		break;

	case 0xF0:
		cpu_emu_data->state = branch_operation;
		cpu_emu_data->address_mode = relative;
		cpu_emu_data->instruction_ptr = &BEQ;
		break;

	case 0x2C:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &BIT;
		break;

	case 0x24:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &BIT;
		break;

	case 0x30:
		cpu_emu_data->state = branch_operation;
		cpu_emu_data->address_mode = relative;
		cpu_emu_data->instruction_ptr = &BMI;
		break;

	case 0xD0:
		cpu_emu_data->state = branch_operation;
		cpu_emu_data->address_mode = relative;
		cpu_emu_data->instruction_ptr = &BNE;
		break;

	case 0x10:
		cpu_emu_data->state = branch_operation;
		cpu_emu_data->address_mode = relative;
		cpu_emu_data->instruction_ptr = &BPL;
		break;

	case 0x00:
		cpu_emu_data->state = break_operation_irq;
		cpu_emu_data->address_mode = implied;
		//cpu_emu_data->instruction_ptr = &BRK;
		break;

	case 0x50:
		cpu_emu_data->state = branch_operation;
		cpu_emu_data->address_mode = relative;
		cpu_emu_data->instruction_ptr = &BVC;
		break;

	case 0x70:
		cpu_emu_data->state = branch_operation;
		cpu_emu_data->address_mode = relative;
		cpu_emu_data->instruction_ptr = &BVS;
		break;

	case 0x18:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &CLC;
		break;

	case 0xD8:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &CLD;
		break;

	case 0x58:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &CLI;
		break;

	case 0xB8:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &CLV;
		break;

	case 0xCD:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &CMP;
		break;

	case 0xDD:
		cpu_emu_data->state = read_execute_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &CMP;
		break;

	case 0xD9:
		cpu_emu_data->state = read_execute_absolute_y;
		cpu_emu_data->address_mode = absolute_y;
		cpu_emu_data->instruction_ptr = &CMP;
		break;

	case 0xC9:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &CMP;
		break;

	case 0xC1:
		cpu_emu_data->state = read_execute_indirect_x;
		cpu_emu_data->address_mode = indexed_indirect_x;
		cpu_emu_data->instruction_ptr = &CMP;
		break;

	case 0xD1:
		cpu_emu_data->state = read_execute_indirect_y;
		cpu_emu_data->address_mode = indirect_indexed_y;
		cpu_emu_data->instruction_ptr = &CMP;
		break;

	case 0xC5:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &CMP;
		break;

	case 0xD5:
		cpu_emu_data->state = read_execute_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &CMP;
		break;

	case 0xEC:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &CPX;
		break;

	case 0xE0:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &CPX;
		break;

	case 0xE4:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &CPX;
		break;

	case 0xCC:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &CPY;
		break;

	case 0xC0:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &CPY;
		break;

	case 0xC4:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &CPY;
		break;

	case 0xCE:
		cpu_emu_data->state = modify_write_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &DEC;
		break;

	case 0xDE:
		cpu_emu_data->state = modify_write_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &DEC;
		break;

	case 0xC6:
		cpu_emu_data->state = modify_write_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &DEC;
		break;

	case 0xD6:
		cpu_emu_data->state = modify_write_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &DEC;
		break;

	case 0xCA:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &DEX;
		break;

	case 0x88:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &DEY;
		break;

	case 0x4D:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &EOR;
		break;

	case 0x5D:
		cpu_emu_data->state = read_execute_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &EOR;
		break;

	case 0x59:
		cpu_emu_data->state = read_execute_absolute_y;
		cpu_emu_data->address_mode = absolute_y;
		cpu_emu_data->instruction_ptr = &EOR;
		break;

	case 0x49:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &EOR;
		break;

	case 0x41:
		cpu_emu_data->state = read_execute_indirect_x;
		cpu_emu_data->address_mode = indexed_indirect_x;
		cpu_emu_data->instruction_ptr = &EOR;
		break;

	case 0x51:
		cpu_emu_data->state = read_execute_indirect_y;
		cpu_emu_data->address_mode = indirect_indexed_y;
		cpu_emu_data->instruction_ptr = &EOR;
		break;

	case 0x45:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &EOR;
		break;

	case 0x55:
		cpu_emu_data->state = read_execute_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &EOR;
		break;

	case 0xEE:
		cpu_emu_data->state = modify_write_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &INC;
		break;

	case 0xFE:
		cpu_emu_data->state = modify_write_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &INC;
		break;

	case 0xE6:
		cpu_emu_data->state = modify_write_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &INC;
		break;

	case 0xF6:
		cpu_emu_data->state = modify_write_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &INC;
		break;

	case 0xE8:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &INX;
		break;

	case 0xC8:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &INY;
		break;

	case 0x4C:
		cpu_emu_data->state = jump_operation_absolute;
		cpu_emu_data->address_mode = absolute;
		//cpu_emu_data->instruction_ptr = &JMP;
		break;

	case 0x6C:
		cpu_emu_data->state = jump_operation_indirect;
		cpu_emu_data->address_mode = indirect;
		//cpu_emu_data->instruction_ptr = &JMP;
		break;

	case 0x20:
		cpu_emu_data->state = jump_to_subroutine;
		cpu_emu_data->address_mode = absolute;
		//cpu_emu_data->instruction_ptr = &JSR;
		break;

	case 0xAD:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &LDA;
		break;

	case 0xBD:
		cpu_emu_data->state = read_execute_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &LDA;
		break;

	case 0xB9:
		cpu_emu_data->state = read_execute_absolute_y;
		cpu_emu_data->address_mode = absolute_y;
		cpu_emu_data->instruction_ptr = &LDA;
		break;

	case 0xA9:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &LDA;
		break;

	case 0xA1:
		cpu_emu_data->state = read_execute_indirect_x;
		cpu_emu_data->address_mode = indexed_indirect_x;
		cpu_emu_data->instruction_ptr = &LDA;
		break;

	case 0xB1:
		cpu_emu_data->state = read_execute_indirect_y;
		cpu_emu_data->address_mode = indirect_indexed_y;
		cpu_emu_data->instruction_ptr = &LDA;
		break;

	case 0xA5:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &LDA;
		break;

	case 0xB5:
		cpu_emu_data->state = read_execute_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &LDA;
		break;

	case 0xAE:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &LDX;
		break;

	case 0xBE:
		cpu_emu_data->state = read_execute_absolute_y;
		cpu_emu_data->address_mode = absolute_y;
		cpu_emu_data->instruction_ptr = &LDX;
		break;

	case 0xA2:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &LDX;
		break;

	case 0xA6:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &LDX;
		break;

	case 0xB6:
		cpu_emu_data->state = read_execute_zeropage_y;
		cpu_emu_data->address_mode = zero_page_y;
		cpu_emu_data->instruction_ptr = &LDX;
		break;

	case 0xAC:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &LDY;
		break;

	case 0xBC:
		cpu_emu_data->state = read_execute_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &LDY;
		break;

	case 0xA0:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &LDY;
		break;

	case 0xA4:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &LDY;
		break;

	case 0xB4:
		cpu_emu_data->state = read_execute_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &LDY;
		break;

	case 0x4E:
		cpu_emu_data->state = modify_write_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &LSR;
		break;

	case 0x5E:
		cpu_emu_data->state = modify_write_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &LSR;
		break;

	case 0x4A:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = accumulator;
		cpu_emu_data->instruction_ptr = &LSR;
		break;

	case 0x46:
		cpu_emu_data->state = modify_write_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &LSR;
		break;

	case 0x56:
		cpu_emu_data->state = modify_write_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &LSR;
		break;

	case 0xEA:
		cpu_emu_data->state = nop_operation;
		cpu_emu_data->address_mode = implied;
		//cpu_emu_data->instruction_ptr = &NOP;
		break;

	case 0x0D:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &ORA;
		break;

	case 0x1D:
		cpu_emu_data->state = read_execute_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &ORA;
		break;

	case 0x19:
		cpu_emu_data->state = read_execute_absolute_y;
		cpu_emu_data->address_mode = absolute_y;
		cpu_emu_data->instruction_ptr = &ORA;
		break;

	case 0x09:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &ORA;
		break;

	case 0x01:
		cpu_emu_data->state = read_execute_indirect_x;
		cpu_emu_data->address_mode = indexed_indirect_x;
		cpu_emu_data->instruction_ptr = &ORA;
		break;

	case 0x11:
		cpu_emu_data->state = read_execute_indirect_y;
		cpu_emu_data->address_mode = indirect_indexed_y;
		cpu_emu_data->instruction_ptr = &ORA;
		break;

	case 0x05:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &ORA;
		break;

	case 0x15:
		cpu_emu_data->state = read_execute_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &ORA;
		break;

	case 0x48:
		cpu_emu_data->state = push_operation_pha;
		cpu_emu_data->address_mode = implied;
		//cpu_emu_data->instruction_ptr = &PHA;
		break;

	case 0x08:
		cpu_emu_data->state = push_operation_php;
		cpu_emu_data->address_mode = implied;
		//cpu_emu_data->instruction_ptr = &PHP;
		break;

	case 0x68:
		cpu_emu_data->state = pull_operation_pla;
		cpu_emu_data->address_mode = implied;
		//cpu_emu_data->instruction_ptr = &PLA;
		break;

	case 0x28:
		cpu_emu_data->state = pull_operation_plp;
		cpu_emu_data->address_mode = implied;
		//cpu_emu_data->instruction_ptr = &PLP;
		break;

	case 0x2E:
		cpu_emu_data->state = modify_write_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &ROL;
		break;

	case 0x3E:
		cpu_emu_data->state = modify_write_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &ROL;
		break;

	case 0x2A:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = accumulator;
		cpu_emu_data->instruction_ptr = &ROL;
		break;

	case 0x26:
		cpu_emu_data->state = modify_write_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &ROL;
		break;

	case 0x36:
		cpu_emu_data->state = modify_write_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &ROL;
		break;

	case 0x6E:
		cpu_emu_data->state = modify_write_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &ROR;
		break;

	case 0x7E:
		cpu_emu_data->state = modify_write_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &ROR;
		break;

	case 0x6A:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = accumulator;
		cpu_emu_data->instruction_ptr = &ROR;
		break;

	case 0x66:
		cpu_emu_data->state = modify_write_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &ROR;
		break;

	case 0x76:
		cpu_emu_data->state = modify_write_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &ROR;
		break;

	case 0x40:
		cpu_emu_data->state = rtn_from_interrupt;
		cpu_emu_data->address_mode = implied;
		//cpu_emu_data->instruction_ptr = &RTI;
		break;

	case 0x60:
		cpu_emu_data->state = rtn_from_subroutine;
		cpu_emu_data->address_mode = implied;
		//cpu_emu_data->instruction_ptr = &RTS;
		break;

	case 0xED:
		cpu_emu_data->state = read_execute_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &SBC;
		break;

	case 0xFD:
		cpu_emu_data->state = read_execute_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &SBC;
		break;

	case 0xF9:
		cpu_emu_data->state = read_execute_absolute_y;
		cpu_emu_data->address_mode = absolute_y;
		cpu_emu_data->instruction_ptr = &SBC;
		break;

	case 0xE9:
		cpu_emu_data->state = read_execute_immediate;
		cpu_emu_data->address_mode = immediate;
		cpu_emu_data->instruction_ptr = &SBC;
		break;

	case 0xE1:
		cpu_emu_data->state = read_execute_indirect_x;
		cpu_emu_data->address_mode = indexed_indirect_x;
		cpu_emu_data->instruction_ptr = &SBC;
		break;

	case 0xF1:
		cpu_emu_data->state = read_execute_indirect_y;
		cpu_emu_data->address_mode = indirect_indexed_y;
		cpu_emu_data->instruction_ptr = &SBC;
		break;

	case 0xE5:
		cpu_emu_data->state = read_execute_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &SBC;
		break;

	case 0xF5:
		cpu_emu_data->state = read_execute_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &SBC;
		break;

	case 0x38:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &SEC;
		break;

	case 0xF8:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &SED;
		break;

	case 0x78:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &SEI;
		break;

	case 0x8D:
		cpu_emu_data->state = store_operation_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &STA;
		break;

	case 0x9D:
		cpu_emu_data->state = store_operation_absolute_x;
		cpu_emu_data->address_mode = absolute_x;
		cpu_emu_data->instruction_ptr = &STA;
		break;

	case 0x99:
		cpu_emu_data->state = store_operation_absolute_y;
		cpu_emu_data->address_mode = absolute_y;
		cpu_emu_data->instruction_ptr = &STA;
		break;

	case 0x81:
		cpu_emu_data->state = store_operation_indirect_x;
		cpu_emu_data->address_mode = indexed_indirect_x;
		cpu_emu_data->instruction_ptr = &STA;
		break;

	case 0x91:
		cpu_emu_data->state = store_operation_indirect_y;
		cpu_emu_data->address_mode = indirect_indexed_y;
		cpu_emu_data->instruction_ptr = &STA;
		break;

	case 0x85:
		cpu_emu_data->state = store_operation_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &STA;
		break;

	case 0x95:
		cpu_emu_data->state = store_operation_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &STA;
		break;

	case 0x8E:
		cpu_emu_data->state = store_operation_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &STX;
		break;

	case 0x86:
		cpu_emu_data->state = store_operation_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &STX;
		break;

	case 0x96:
		cpu_emu_data->state = store_operation_zeropage_y;
		cpu_emu_data->address_mode = zero_page_y;
		cpu_emu_data->instruction_ptr = &STX;
		break;

	case 0x84:
		cpu_emu_data->state = store_operation_zeropage;
		cpu_emu_data->address_mode = zero_page;
		cpu_emu_data->instruction_ptr = &STY;
		break;

	case 0x94:
		cpu_emu_data->state = store_operation_zeropage_x;
		cpu_emu_data->address_mode = zero_page_x;
		cpu_emu_data->instruction_ptr = &STY;
		break;

	case 0x8C:
		cpu_emu_data->state = store_operation_absolute;
		cpu_emu_data->address_mode = absolute;
		cpu_emu_data->instruction_ptr = &STY;
		break;

	case 0xAA:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &TAX;
		break;

	case 0xA8:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &TAY;
		break;

	case 0xBA:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &TSX;
		break;

	case 0x8A:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &TXA;
		break;

	case 0x9A:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &TXS;
		break;

	case 0x98:
		cpu_emu_data->state = single_byte_intstruction;
		cpu_emu_data->address_mode = implied;
		cpu_emu_data->instruction_ptr = &TYA;
		break;
	}
}
//********************************************************************************
//********************************************************************************




//************************************************************************************************
//************************************************************************************************
//********** CPU INSTRUCTION EXECUTION FUNCTIONS *************************************************
void ADC(cpu_emu_dat *cpu_emu_data) {

	//ADC - Add memory to accumulator with carry
	//NOTE: NES doesn't make use of decimal mode
	//Affects flags - N,Z,C,V 
	unsigned short temp;
	unsigned char c6 = ((cpu_emu_data->data & (1 << 6)) & (CPU_REGISTERS.ACC_REG & (1 << 6)));
	
	CPU_REGISTERS.ACC_REG += cpu_emu_data->data + CPU_REGISTERS.CPU_STATUS_REG.BIT.C;
	temp = CPU_REGISTERS.ACC_REG + cpu_emu_data->data + CPU_REGISTERS.CPU_STATUS_REG.BIT.C;
	Clear_carry_flag();

	if (temp > 0x00FF) {
		Set_carry_flag();
	}

	Update_zero_flag(CPU_REGISTERS.ACC_REG);
	Update_negative_flag(CPU_REGISTERS.ACC_REG);
	CPU_REGISTERS.CPU_STATUS_REG.BIT.V = Check_carry_flag() ^ c6;
}


void AND(cpu_emu_dat *cpu_emu_data) {

	//AND - 'AND' memory with accumulator
	//Affects flags - N,Z
	CPU_REGISTERS.ACC_REG &= cpu_emu_data->data;
	Update_negative_flag(CPU_REGISTERS.ACC_REG);
	Update_zero_flag(CPU_REGISTERS.ACC_REG);
}


void ASL(cpu_emu_dat *cpu_emu_data) {

	//ASL - Shift memory or accumulator left by one,
	//carry flag is set to value of bit 7 of operand.
	//Affects flags - N,Z,C
	if (cpu_emu_data->address_mode == accumulator) {
		CPU_REGISTERS.CPU_STATUS_REG.BIT.C = (CPU_REGISTERS.ACC_REG & (1 << 7));
		CPU_REGISTERS.ACC_REG <<= 1;
		Update_zero_flag(CPU_REGISTERS.ACC_REG);
		Update_negative_flag(CPU_REGISTERS.ACC_REG);
	}
	else {
		CPU_REGISTERS.CPU_STATUS_REG.BIT.C = (cpu_emu_data->data & (1 << 7));
		cpu_emu_data->data <<= 1;
		Update_zero_flag(cpu_emu_data->data);
		Update_negative_flag(cpu_emu_data->data);
	}
}


void BCC(cpu_emu_dat *cpu_emu_data) {

	//BCC - Branch on carry clear (if C = 0)
	//Affects flags - NONE
	if ( Check_carry_flag() == 0 ) {
		cpu_emu_data->branch_taken = true;
	}
}


void BCS(cpu_emu_dat *cpu_emu_data) {

	//BCS - Branch on carry set (if C = 1)
	//Affects flags - NONE
	if ( Check_carry_flag() == 1 ) {
		cpu_emu_data->branch_taken = true;
	}
}


void BEQ(cpu_emu_dat *cpu_emu_data) {

	//BEQ - Branch on result zero (if Z = 1)
	//Affects flags - NONE
	if ( Check_zero_flag() == 1 ) {
		cpu_emu_data->branch_taken = true;
	}
}


void BIT(cpu_emu_dat *cpu_emu_data) {

	//BIT - 'AND' memory with accumulator, if result 0, set flag Z = 1.
	//Also, values of memory bits M7 and M6 are copied to flags N,V
	//Neither accumulator nor memory data are altered!
	//Affects flags - N,Z,V
	unsigned char temp = (CPU_REGISTERS.ACC_REG & cpu_emu_data->data);
	CPU_REGISTERS.CPU_STATUS_REG.BIT.N = (cpu_emu_data->data & (1 << 7));
	CPU_REGISTERS.CPU_STATUS_REG.BIT.V = (cpu_emu_data->data & (1 << 6));
	Update_zero_flag(temp);
}


void BMI(cpu_emu_dat *cpu_emu_data) {

	//BMI - Branch on result minus (branch if flag N = 1)
	//Affects flags - NONE
	if ( Check_negative_flag() == 1 ) {
		cpu_emu_data->branch_taken = true;
	}
}


void BNE(cpu_emu_dat *cpu_emu_data) {

	//BNE - Branch on result not zero (branch if flag Z = 0)
	//Affects flags - NONE
	if ( Check_zero_flag() == 0 ) {
		cpu_emu_data->branch_taken = true;
	}
}


void BPL(cpu_emu_dat *cpu_emu_data) {

	//BPL - Branch on result positive (branch if flag N = 0)
	//Affects flags - NONE
	if ( Check_negative_flag() == 0 ) {
		cpu_emu_data->branch_taken = true;
	}
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
	if ( Check_overflow_flag() == 0 ) {
		cpu_emu_data->branch_taken = true;
	}
}


void BVS(cpu_emu_dat *cpu_emu_data) {

	//BVS - Branch on overflow set (branch if flag V = 1)
	//Affects flags - NONE
	if ( Check_overflow_flag() == 1 ) {
		cpu_emu_data->branch_taken = true;
	}
}


void CLC(cpu_emu_dat *cpu_emu_data) {

	//CLC - Clear carry flag
	//Affects flags - C
	Clear_carry_flag();
}


void CLD(cpu_emu_dat *cpu_emu_data) {

	//CLD - Clear decimal flag
	//Affects flags - D
	Clear_decimal_flag();
}


void CLI(cpu_emu_dat *cpu_emu_data) {

	//CLI - Clear IRQ disable flag
	//Affects flags - I
	Clear_interrupt_flag();
}


void CLV(cpu_emu_dat *cpu_emu_data) {

	//CLV - Clear overflow flag
	//Affects flags - V
	Clear_overflow_flag();
}


void CMP(cpu_emu_dat *cpu_emu_data) {

	//CMP - Subtract memory from accumulator to compare values. Result not kept.
	//Set flag C if A >= M, Set flag Z if A = M, Set flag N if bit 7 of result is set 
	//Affects flags - N,Z,C
	unsigned char result = CPU_REGISTERS.ACC_REG + cpu_emu_data->data;
	if (CPU_REGISTERS.ACC_REG >= cpu_emu_data->data) Set_carry_flag();
	if (CPU_REGISTERS.ACC_REG == cpu_emu_data->data) Set_zero_flag();
	if (result && (1 << 7)) Set_negative_flag();
}


void CPX(cpu_emu_dat *cpu_emu_data) {

	//CMP - Subtract memory from X register to compare values. Result not kept.
	//Set flag C if X >= M, Set flag Z if X = M, Set flag N if bit 7 of result is set 
	//Affects flags - N,Z,C
	unsigned char result = CPU_REGISTERS.X_REG + cpu_emu_data->data;
	if (CPU_REGISTERS.X_REG >= cpu_emu_data->data) Set_carry_flag();
	if (CPU_REGISTERS.X_REG == cpu_emu_data->data) Set_zero_flag();
	if (result && (1 << 7)) Set_negative_flag();
}


void CPY(cpu_emu_dat *cpu_emu_data) {

	//CMP - Subtract memory from Y register to compare values. Result not kept.
	//Set flag C if Y >= M, Set flag Z if Y = M, Set flag N if bit 7 of result is set 
	//Affects flags - N,Z,C
	unsigned char result = CPU_REGISTERS.Y_REG + cpu_emu_data->data;
	if (CPU_REGISTERS.Y_REG >= cpu_emu_data->data) Set_carry_flag();
	if (CPU_REGISTERS.Y_REG == cpu_emu_data->data) Set_zero_flag();
	if (result && (1 << 7)) Set_negative_flag();
}


void DEC(cpu_emu_dat *cpu_emu_data) {

	//DEC - Decrement memory. Subtract one from the value held in memory (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	cpu_emu_data->data--;
	Update_negative_flag(cpu_emu_data->data);
	Update_zero_flag(cpu_emu_data->data);
}


void DEX(cpu_emu_dat *cpu_emu_data) {

	//DEX - Decrement value in X register by one (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z, N
	CPU_REGISTERS.X_REG--;
	Update_negative_flag(CPU_REGISTERS.X_REG);
	Update_zero_flag(CPU_REGISTERS.X_REG);
}


void DEY(cpu_emu_dat *cpu_emu_data) {

	//DEX - Decrement value in Y register by one (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.Y_REG--;
	Update_negative_flag(CPU_REGISTERS.Y_REG);
	Update_zero_flag(CPU_REGISTERS.Y_REG);
}


void EOR(cpu_emu_dat *cpu_emu_data) {

	//EOR - Exclusive or memory and accumulator (result is kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.ACC_REG ^= cpu_emu_data->data;
	Update_zero_flag(CPU_REGISTERS.ACC_REG);
	Update_negative_flag(CPU_REGISTERS.ACC_REG);
}


void INC(cpu_emu_dat *cpu_emu_data) {

	//INC - Increment memory. Add one to the value stored in memory (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N 
	cpu_emu_data->data++;
	Update_zero_flag(cpu_emu_data->data);
	Update_negative_flag(cpu_emu_data->data);
}


void INX(cpu_emu_dat *cpu_emu_data) {

	//DEX - Increment value in X register by one (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.X_REG++;
	Update_negative_flag(CPU_REGISTERS.X_REG);
	Update_zero_flag(CPU_REGISTERS.X_REG);
}


void INY(cpu_emu_dat *cpu_emu_data) {

	//DEY - Increment value in Y register by one (result kept).
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.Y_REG++;
	Update_negative_flag(CPU_REGISTERS.Y_REG);
	Update_zero_flag(CPU_REGISTERS.Y_REG);
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
	Update_zero_flag(CPU_REGISTERS.ACC_REG);
	Update_negative_flag(CPU_REGISTERS.ACC_REG);
}


void LDX(cpu_emu_dat *cpu_emu_data) {

	//LDX - Load X register. Loads a byte of memory into the X register.
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.X_REG = cpu_emu_data->data;
	Update_zero_flag(CPU_REGISTERS.X_REG);
	Update_negative_flag(CPU_REGISTERS.X_REG);
}


void LDY(cpu_emu_dat *cpu_emu_data) {

	//LDY - Load Y register. Loads a byte of memory into the Y register.
	//Set flag Z if result 0, Set flag N if bit 7 of result is set.
	//Affects flags - Z,N
	CPU_REGISTERS.Y_REG = cpu_emu_data->data;
	Update_zero_flag(CPU_REGISTERS.Y_REG);
	Update_negative_flag(CPU_REGISTERS.Y_REG);
}


void LSR(cpu_emu_dat *cpu_emu_data) {

	//LSR - Logical shift right. The accumulator or memory location specified is shifted right by one.
	//The bit that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero (N flag).
	//Affects flags - C,Z,N
	if (cpu_emu_data->address_mode == accumulator) {
		CPU_REGISTERS.CPU_STATUS_REG.BIT.C = CPU_REGISTERS.ACC_REG & 0x01;
		CPU_REGISTERS.ACC_REG >>= 1;
		Update_zero_flag(CPU_REGISTERS.ACC_REG);
	}
	else {
		CPU_REGISTERS.CPU_STATUS_REG.BIT.C = cpu_emu_data->data & 0x01;
		cpu_emu_data->data >>= 1;
		Update_zero_flag(cpu_emu_data->data);
	}
	Clear_zero_flag();
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
	Update_zero_flag(CPU_REGISTERS.ACC_REG);
	Update_negative_flag(CPU_REGISTERS.ACC_REG);
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


/*void PLA(cpu_emu_dat *cpu_emu_data) {

	//This operation is handled entirely by the state machine!
	//PLA - Pull accumulator from the stack.
	//Affects flags - Z,N
}*/


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
	unsigned char temp = CPU_REGISTERS.CPU_STATUS_REG.BIT.C;

	if (cpu_emu_data->address_mode == accumulator) {
		CPU_REGISTERS.CPU_STATUS_REG.BIT.C = (CPU_REGISTERS.ACC_REG & (1 << 7));
		CPU_REGISTERS.ACC_REG <<= 1;
		CPU_REGISTERS.ACC_REG |= temp;
		Update_negative_flag(CPU_REGISTERS.ACC_REG);
		Update_zero_flag(CPU_REGISTERS.ACC_REG);
	}
	else {
		CPU_REGISTERS.CPU_STATUS_REG.BIT.C = (cpu_emu_data->data & (1 << 7));
		cpu_emu_data->data <<= 1;
		cpu_emu_data->data |= temp;
		Update_negative_flag(cpu_emu_data->data);
		Update_zero_flag(cpu_emu_data->data);
	}
}


void ROR(cpu_emu_dat *cpu_emu_data) {

	//ROR - Rotate memory or accumulator one bit right.
	//Bit 7 is filled with the current value of the carry flag
	//whilst the old bit 0 becomes the new carry flag value.
	//Affects flags - Z,N,C
	unsigned char temp = CPU_REGISTERS.CPU_STATUS_REG.BIT.C;

	if (cpu_emu_data->address_mode == accumulator) {
		CPU_REGISTERS.CPU_STATUS_REG.BIT.C = (CPU_REGISTERS.ACC_REG & 0x01);
		CPU_REGISTERS.ACC_REG >>= 1;
		CPU_REGISTERS.ACC_REG |= (temp << 7);
		Update_negative_flag(CPU_REGISTERS.ACC_REG);
		Update_zero_flag(CPU_REGISTERS.ACC_REG);
	}
	else {
		CPU_REGISTERS.CPU_STATUS_REG.BIT.C = (cpu_emu_data->data & 0x01);
		cpu_emu_data->data >>= 1;
		cpu_emu_data->data |= (temp << 7);
		Update_negative_flag(cpu_emu_data->data);
		Update_zero_flag(cpu_emu_data->data);
	}
}


/*void RTI(cpu_emu_dat *cpu_emu_data) {

	//This operation is handled entirely by the state machine!
	//RTI - Return from interrupt.
	//Pull status register from stack, followed by program counter.
}*/


/*void RTS(cpu_emu_dat *cpu_emu_data) {

	//This operation is handled entirely by the state machine!
	//RTS - Return from subroutine.
	//Pull program counter from stack and add one to it (careful with this)
}*/


void SBC(cpu_emu_dat *cpu_emu_data) {

	//SBC - Subtract with carry. The Carry flag in status reg will always
	//be set before this instruction is called. If the carry flag bit
	//is borrow from, it's value is 256 (and it will be cleared).
	//Affects flags - Z,N,C,V
	cpu_emu_data->data ^= 0xFF;	//Ones complicment
	ADC(cpu_emu_data);
}


void SEC(cpu_emu_dat *cpu_emu_data) {

	//SEC - Set carry flag.
	//Affects flags - C
	Set_carry_flag();
}


void SED(cpu_emu_dat *cpu_emu_data) {

	//SED - Set decimal flag.
	//Affects flags - D
	Set_decimal_flag();
}


void SEI(cpu_emu_dat *cpu_emu_data) {

	//SEI - Set IRQ disable flag.
	//Affects flags - I
	Set_interrupt_flag();
}


void STA(cpu_emu_dat *cpu_emu_data) {

	//STA - Store acc. Stores value of accumulator to memory.
	//Affects flags - NONE
	cpu_emu_data->data = CPU_REGISTERS.ACC_REG;
}


void STX(cpu_emu_dat *cpu_emu_data) {

	//STX - Store X reg. Stores value of X register to memory.
	//Affects flags - NONE
	cpu_emu_data->data = CPU_REGISTERS.X_REG;
}


void STY(cpu_emu_dat *cpu_emu_data) {

	//STY - Store Y reg. Stores value of Y register to memory.
	//Affects flags - NONE
	cpu_emu_data->data = CPU_REGISTERS.Y_REG;
}


void TAX(cpu_emu_dat *cpu_emu_data) {

	//TAX - Transfer accumulator to X register. 
	//Affects flags - Z,N
	CPU_REGISTERS.X_REG = CPU_REGISTERS.ACC_REG;
	Update_negative_flag(CPU_REGISTERS.X_REG);
	Update_zero_flag(CPU_REGISTERS.X_REG);
}


void TAY(cpu_emu_dat *cpu_emu_data) {

	//TAY - Transfer accumulator to Y register. 
	//Affects flags - Z,N
	CPU_REGISTERS.Y_REG = CPU_REGISTERS.ACC_REG;
	Update_negative_flag(CPU_REGISTERS.Y_REG);
	Update_zero_flag(CPU_REGISTERS.Y_REG);
}


void TSX(cpu_emu_dat *cpu_emu_data) {

	//TSX - Transfer stack pointer to X register.
	//Affects flags - Z,N (confirm this)
	CPU_REGISTERS.X_REG = CPU_REGISTERS.STACK_POINTER;
	Update_negative_flag(CPU_REGISTERS.X_REG);
	Update_zero_flag(CPU_REGISTERS.X_REG);
}


void TXA(cpu_emu_dat *cpu_emu_data) {

	//TXA - Tansfer X register to accumulator.
	//Affects flags - Z,N
	CPU_REGISTERS.ACC_REG = CPU_REGISTERS.X_REG;
	Update_negative_flag(CPU_REGISTERS.ACC_REG);
	Update_zero_flag(CPU_REGISTERS.ACC_REG);
}


void TXS(cpu_emu_dat *cpu_emu_data) {

	//TXS - Transfer X register to stack pointer.
	//Affects flags - NONE
	CPU_REGISTERS.STACK_POINTER = CPU_REGISTERS.X_REG;
}


void TYA(cpu_emu_dat *cpu_emu_data) {

	//TYA - Transfer Y register into the accumulator.
	//Affects flags - Z,N
	CPU_REGISTERS.ACC_REG = CPU_REGISTERS.Y_REG;
	Update_negative_flag(CPU_REGISTERS.ACC_REG);
	Update_zero_flag(CPU_REGISTERS.ACC_REG);
}
//************************************************************************************************
//************************************************************************************************
//************************************************************************************************



