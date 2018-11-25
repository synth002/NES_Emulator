#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NES.h"
#include "NES_CPU_Shared.h"

#define NIBBLE_SHIFT 0x04
#define NIBBLE_MASK  0x0F

//***Local globals*** 
ROM_data		ROM_DATA;
unsigned char	NES_MEMORY[65536];



unsigned char Setup_emulator(char *ROM_header) {	

	/*
	This function handles setup of the emulator before any system emulation begins, 
	it is to be called by the host system before any other interactions with the
	emulator can take place. 

	The function requires an iNES header data string to be passed to it, which should
	conistst of the following: 

	(15 bytes total)
	ROM_header[0-2]  - Bytes 0 through 2 of the iNES ROM file.
	ROM_header[3-14] - Bytes 4 through 15 of the iNES ROM file.

	If setup was sucessful (a valid iNES header was detected) the function will 
	return 1. If setup fails (invaild header is detected) function will return 0.
	*/
	
	unsigned char a;
	char *iNES_identifier = "NES";

	//Check if ROM is valid iNES format
	for (a = 0; a <= 2; a++) {
		if (iNES_identifier[a] != ROM_header[a]) return 0;
	}

	//Define ROM data parameters
	ROM_DATA.PRG_ROM_SIZE = ROM_header[3];
	ROM_DATA.CHR_ROM_SIZE = ROM_header[4];
	ROM_DATA.FLAGS_6 = ROM_header[5];
	ROM_DATA.FLAGS_7 = ROM_header[6];
	ROM_DATA.MAPPER_NUMBER = (ROM_DATA.FLAGS_6 >> NIBBLE_SHIFT);
	ROM_DATA.MAPPER_NUMBER |= (ROM_DATA.FLAGS_7 & (NIBBLE_MASK << NIBBLE_SHIFT));

	/*
	//***** For testing purposes only! ******************
	printf("MAPPER_NUMBER = %d\n", ROM_DATA.MAPPER_NUMBER);
	printf("PRG_ROM_SIZE = %d\n", ROM_DATA.PRG_ROM_SIZE);
	printf("CHR_ROM_SIZE = %d\n", ROM_DATA.CHR_ROM_SIZE);
	printf("Flags_6 = %d\n", ROM_DATA.FLAGS_6);
	printf("Flags_7 = %d\n", ROM_DATA.FLAGS_7);
	//***************************************************
	*/
	

	Setup_CPU();
	//Setup_PPU();
	//Setup_APU();

	return 1;
};


void Emulator_action_tick(void) {

	CPU_cycle();
	//PPU_cycle();
	//APU_cycle();

};





















	









