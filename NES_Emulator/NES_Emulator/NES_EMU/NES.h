#pragma once



/*
	This should be the only header file required by the host system
	in order to run the NES emulator!
*/


//***ROM header data***
typedef struct {
	unsigned char NES_IDENTIFIER[4];
	unsigned char MAPPER_NUMBER;
	unsigned char PRG_ROM_SIZE;
	unsigned char CHR_ROM_SIZE;
	unsigned char FLAGS_6;
	unsigned char FLAGS_7;
}ROM_data;


//Should be called by host at startup or system reset
extern unsigned char Setup_emulator(char *ROM_header);

//Should be called by host at fixed timing intervals
extern void Emulator_action_tick(void);