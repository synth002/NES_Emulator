#pragma once
#include "NES.h"

/*
	This header should contain definitions and function
	prototypes that are local to the CPU only!
*/


//**** CPU attributes ****
#define TOP_OF_STACK			0x01FF	
#define RAM_BASE_ADDR			0x0200
#define STACK_BASE_ADDR			0x0100
#define ZERO_PAGE_BASE_ADDR		0x0000
#define CYCLES_PER_FRAME		0x0000
#define RESET_VECTOR_ADDR		0xFFFC	
#define STATUS_REG_RESET		0x34





typedef enum
{
	fetch_op,
	write_op,
	fetch_previous
}rw;


typedef enum
{
	false = 0,
	true  = 1,
	none  = 2
}bool;


typedef enum
{
	no_irq, 
	reset
} interrupts;


typedef enum
{
	immediate,
	zero_page,
	zero_page_x,
	zero_page_y,
	absolute,
	absolute_x,
	absolute_y,
	indexed_indirect_x,
	indirect_indexed_y,
	accumulator,
	relative,
	implied,
	indirect
}addr_mode;



typedef union
{
	unsigned short reg;
	struct
	{
		unsigned char low  : 8;
		unsigned char high : 8;

	} byte;
} address_value;


typedef union
{
	unsigned short REG;
	struct
	{
		unsigned char LOW  : 8;
		unsigned char HIGH : 8;

	} BYTE;
} program_counter;


typedef union 
{
	unsigned char reg;
	struct
	{
		unsigned char mirroring		   : 1;
		unsigned char battery		   : 1;
		unsigned char trainer		   : 1;
		unsigned char ignore_mirroring : 1;
		unsigned char mapper_lsn	   : 4;
	} flag;
} flags_six;


typedef union
{
	unsigned char reg;
	struct
	{
		unsigned char vs_unisystem	: 1;
		unsigned char playchoice	: 1;
		unsigned char ines_2		: 2;
		unsigned char mapper_msn	: 4;
	} flag;
} flags_seven;


//***ROM header data***
typedef struct {
	unsigned char nes_identifier[4];
	unsigned char mapper_number;
	unsigned char prg_rom_size;
	unsigned char chr_rom_size;
	flags_six     flags_6;
	flags_seven   flags_7;
	unsigned char prg_ram_size;
	void(*mapper_ptr)(void);
	unsigned short mapper_mask;
}ROM_data;









//****Runtime data****
typedef struct {
	
	unsigned char cycle;
	unsigned char opcode;
	unsigned char irq;
	unsigned char data;
	unsigned char *state;
	bool branch_taken;
	bool page_crossed;
	addr_mode address_mode;
	address_value base_addr;
	address_value indexed_addr;
	void(*instruction_ptr)(cpu_emu_dat);
#ifdef LOGGING_ENABLED
	char log_string[200];
	char addr_string[20];
	char inst_string[4];
	unsigned char F1, F2, F3;
	unsigned short PC;
#endif
}cpu_emu_dat;




//***CPU status reg***
typedef union
{
	unsigned REG;
	struct
	{
		unsigned char C  : 1;			//Carry flag (bit 0)
		unsigned char Z  : 1;			//Zero flag
		unsigned char I  : 1;			//IRQ disabe flag
		unsigned char D  : 1;			//Decimal flag (Not used)
		unsigned char BL : 1;			//B flag low
		unsigned char BH : 1;			//B flag high
		unsigned char V  : 1;			//Overflow flag
		unsigned char N  : 1;			//Negative flag

	} BIT;
} status_reg;




//***CPU registers***
typedef struct {
	unsigned char	ACC_REG;			//Accumulator register
	unsigned char	X_REG;				//CPU index register X
	unsigned char	Y_REG;				//CPU index register Y
	unsigned char	SP;					//CPU stack pointer register (descending stack)
	program_counter	PC;					//CPU program counter register
	status_reg		S_REG;				//CPU status register
	//Unoffical!
	unsigned int	RESET;				//Reset event on true
} CPU_registers;





void Get_mapper_pointer(void);
void Mapper_0(void);

//Local function prototypes
void Init_attributes(cpu_emu_dat *cpu_emu_data);
void Instruction_lookup(cpu_emu_dat *cpu_emu_data);

//External function prototypes

extern void Setup_CPU(void);
extern void CPU_cycle(void);

//CPU low level function prototypes
extern void Update_zero_flag(unsigned char val);
extern void Update_negative_flag(unsigned char val);
extern bool Check_for_page_crossing(unsigned short value);
extern void Update_overflow_flag(unsigned char acc, unsigned char val);
extern unsigned char Memory_access(unsigned char operation, unsigned short addr, unsigned char data);
//Status reg flag functions
extern void Set_negative_flag(void);
extern void Set_zero_flag(void);
extern void Set_carry_flag(void);
extern void Set_interrupt_flag(void);
extern void Set_decimal_flag(void);
extern void Set_overflow_flag(void);
extern void Clear_negative_flag(void);
extern void Clear_zero_flag(void);
extern void Clear_carry_flag(void);
extern void Clear_interrupt_flag(void);
extern void Clear_decimal_flag(void);
extern void Clear_break_flag(void);
extern void Clear_overflow_flag(void);
extern unsigned char Check_negative_flag(void);
extern unsigned char Check_zero_flag(void);
extern unsigned char Check_carry_flag(void);
extern unsigned char Check_interrupt_flag(void);
extern unsigned char Check_decimal_flag(void);
extern unsigned char Check_break_flag(void);
extern unsigned char Check_overflow_flag(void);


//Instruction exection functions
extern void ADC(cpu_emu_dat *cpu_emu_data);
extern void AND(cpu_emu_dat *cpu_emu_data);
extern void ASL(cpu_emu_dat *cpu_emu_data);
extern void BCC(cpu_emu_dat *cpu_emu_data);
extern void BCS(cpu_emu_dat *cpu_emu_data);
extern void BEQ(cpu_emu_dat *cpu_emu_data);
extern void BIT(cpu_emu_dat *cpu_emu_data);
extern void BMI(cpu_emu_dat *cpu_emu_data);
extern void BNE(cpu_emu_dat *cpu_emu_data);
extern void BPL(cpu_emu_dat *cpu_emu_data);
extern void BRK(cpu_emu_dat *cpu_emu_data);
extern void BVC(cpu_emu_dat *cpu_emu_data);
extern void BVS(cpu_emu_dat *cpu_emu_data);
extern void CLC(cpu_emu_dat *cpu_emu_data);
extern void CLD(cpu_emu_dat *cpu_emu_data);
extern void CLI(cpu_emu_dat *cpu_emu_data);
extern void CLV(cpu_emu_dat *cpu_emu_data);
extern void CMP(cpu_emu_dat *cpu_emu_data);
extern void CPX(cpu_emu_dat *cpu_emu_data);
extern void CPY(cpu_emu_dat *cpu_emu_data);
extern void DEC(cpu_emu_dat *cpu_emu_data);
extern void DEX(cpu_emu_dat *cpu_emu_data);
extern void DEY(cpu_emu_dat *cpu_emu_data);
extern void EOR(cpu_emu_dat *cpu_emu_data);
extern void INC(cpu_emu_dat *cpu_emu_data);
extern void INX(cpu_emu_dat *cpu_emu_data);
extern void INY(cpu_emu_dat *cpu_emu_data);
extern void JMP(cpu_emu_dat *cpu_emu_data);
extern void JSR(cpu_emu_dat *cpu_emu_data);
extern void LDA(cpu_emu_dat *cpu_emu_data);
extern void LDX(cpu_emu_dat *cpu_emu_data);
extern void LDY(cpu_emu_dat *cpu_emu_data);
extern void LSR(cpu_emu_dat *cpu_emu_data);
extern void NOP(cpu_emu_dat *cpu_emu_data);
extern void ORA(cpu_emu_dat *cpu_emu_data);
extern void PHA(cpu_emu_dat *cpu_emu_data);
extern void PHP(cpu_emu_dat *cpu_emu_data);
extern void PLA(cpu_emu_dat *cpu_emu_data);
extern void PLP(cpu_emu_dat *cpu_emu_data);
extern void ROL(cpu_emu_dat *cpu_emu_data);
extern void ROR(cpu_emu_dat *cpu_emu_data);
extern void RTI(cpu_emu_dat *cpu_emu_data);
extern void RTS(cpu_emu_dat *cpu_emu_data);
extern void SBC(cpu_emu_dat *cpu_emu_data);
extern void SEC(cpu_emu_dat *cpu_emu_data);
extern void SED(cpu_emu_dat *cpu_emu_data);
extern void SEI(cpu_emu_dat *cpu_emu_data);
extern void STA(cpu_emu_dat *cpu_emu_data);
extern void STX(cpu_emu_dat *cpu_emu_data);
extern void STY(cpu_emu_dat *cpu_emu_data);
extern void TAX(cpu_emu_dat *cpu_emu_data);
extern void TAY(cpu_emu_dat *cpu_emu_data);
extern void TSX(cpu_emu_dat *cpu_emu_data);
extern void TXA(cpu_emu_dat *cpu_emu_data);
extern void TXS(cpu_emu_dat *cpu_emu_data);
extern void TYA(cpu_emu_dat *cpu_emu_data);