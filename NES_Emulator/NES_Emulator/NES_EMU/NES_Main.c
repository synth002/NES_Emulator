#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NES.h"
#include "NES_CPU.h"

#define NIBBLE_SHIFT	0x04
#define NIBBLE_MASK		0x0F
#define SIXTEEN_KB		0x4000
#define MAPPER_0_MASK	0xBFFF
#define NO_MASK         0xFFFF
#define CART_SPACE      0x4020

#define ROM_START_BYTE  16
#define TRAINER_LEN		512




//***Local globals*** 
ROM_data		ROM_DATA;
unsigned char	NES_MEMORY[65536] = { 0 };




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
	ROM_DATA.prg_rom_size   = ROM_header[3];
	ROM_DATA.chr_rom_size   = ROM_header[4];
	ROM_DATA.flags_6.reg    = ROM_header[5];
	ROM_DATA.flags_7.reg    = ROM_header[6];
	ROM_DATA.mapper_number  = ROM_DATA.flags_6.flag.mapper_lsn;
	ROM_DATA.mapper_number |= (ROM_DATA.flags_7.flag.mapper_msn << 4);
	ROM_DATA.prg_ram_size   = ROM_header[7];



	/*
	//***** For testing purposes only! ******************
	printf("MAPPER_NUMBER	= %d\n", ROM_DATA.mapper_number);
	printf("PRG_ROM_SIZE	= %d (x16KB)\n", ROM_DATA.prg_rom_size);
	printf("CHR_ROM_SIZE	= %d (x8KB)\n", ROM_DATA.chr_rom_size);
	printf("CHR_RAM_SIZE	= %d (x8KB)\n", ROM_DATA.prg_ram_size);
	printf("Flags_6		= 0x%.2X\n", ROM_DATA.flags_6.reg);
	printf("Flags_7		= 0x%.2X\n", ROM_DATA.flags_7.reg);
	printf("****************************\n");
	printf("**** Flags 6 Breakdown *****\n");
	printf("Mirroring	 = %d\n", ROM_DATA.flags_6.flag.mirroring);
	printf("Cart Battery	 = %d\n", ROM_DATA.flags_6.flag.battery);
	printf("Rom trainer	 = %d\n", ROM_DATA.flags_6.flag.trainer);
	printf("Ignore mirroring = %d\n", ROM_DATA.flags_6.flag.ignore_mirroring);
	printf("Mapper num LSN	 = %d\n", ROM_DATA.flags_6.flag.mapper_lsn);
	printf("****************************\n");
	printf("**** Flags 7 Breakdown *****\n");
	printf("VS Unisystem     = %d\n", ROM_DATA.flags_7.flag.vs_unisystem);
	printf("Playchoice	 = %d\n", ROM_DATA.flags_7.flag.playchoice);
	printf("iNes 2.0	 = %d\n", ROM_DATA.flags_7.flag.ines_2);
	printf("Mapper num MSN	 = %d\n", ROM_DATA.flags_7.flag.mapper_msn);
	printf("****************************\n");
	//***************************************************
	*/

	//Get function pointer &
	//Initilise ROM mapping
	Get_mapper_pointer();
	ROM_DATA.mapper_ptr();


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




void Get_mapper_pointer(void) {

	switch (ROM_DATA.mapper_number) {


	case 0 :
		ROM_DATA.mapper_ptr = Mapper_0;
		if (ROM_DATA.prg_rom_size > 1) {
			ROM_DATA.mapper_mask = NO_MASK;
		} else {
			ROM_DATA.mapper_mask = MAPPER_0_MASK;
		}
		break;

	case 1 :
		break;


	default:
		//Error
		break;
	}
}




void Mapper_0(void) {

	unsigned int num_bytes;
	unsigned int read_address;
	num_bytes = ROM_DATA.prg_rom_size * SIXTEEN_KB;
	read_address = (ROM_START_BYTE + (ROM_DATA.flags_6.flag.trainer * TRAINER_LEN));
	Grab_rom_data(NES_MEMORY, 0x8000 , num_bytes, read_address);
}