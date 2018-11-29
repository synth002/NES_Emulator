#pragma once


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
	write_op
}rw;


typedef enum
{
	false = 0,
	true  = 1
}bool;


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
	indirect,
	none
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
	bool branch_taken;
	unsigned char opcode;
	addr_mode address_mode;
	void(*instruction_ptr)(cpu_emu_dat);
	unsigned char cylce_counter;
	unsigned char data;
	unsigned char state[10];
	address_value base_addr;
	address_value indexed_addr;
}cpu_emu_dat;




//***CPU status reg***
typedef union
{
	unsigned REG;
	struct
	{
		unsigned char C : 1;			//Carry flag (bit 0)
		unsigned char Z : 1;			//Zero flag
		unsigned char I : 1;			//IRQ disabe flag
		unsigned char D : 1;			//Decimal flag (Not used)
		unsigned char B : 2;			//Break flag
		unsigned char V : 1;			//Overflow flag
		unsigned char N : 1;			//Negative flag

	} BIT;
} status_reg;




//***CPU registers***
typedef struct {
	unsigned char	ACC_REG;			//Accumulator register
	unsigned char	X_REG;				//CPU index register X
	unsigned char	Y_REG;				//CPU index register Y
	unsigned char	STACK_POINTER;		//CPU stack pointer register (descending stack)
	program_counter	PROGRAM_COUNTER;	//CPU program counter register
	status_reg		CPU_STATUS_REG;	    //CPU status register
	//Unoffical!
	unsigned int	CYCLE_COUNT;		//cycle counter - may get removed!
} CPU_registers;





void Get_mapper_pointer(void);
void Mapper_0(void);

//Local function prototypes
void Instruction_lookup(unsigned char OP_CODE, cpu_emu_dat *cpu_emu_data);

//External function prototypes
extern void Update_negative_flag(unsigned char val);
extern void Update_zero_flag(unsigned char val);

extern void Set_negative_flag(void);
extern void Set_zero_flag(void);
extern void Set_carry_flag(void);
extern void Set_interrupt_flag(void);
extern void Set_decimal_flag(void);
extern void Set_break_flag(void);
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


extern unsigned char Memory_access(unsigned char rw, unsigned short addr, unsigned char data);


extern bool Check_for_page_crossing(unsigned short value);
extern unsigned char Fetch_opcode(void);
extern unsigned char Fetch_data(unsigned short addr);


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