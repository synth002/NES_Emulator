
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NES_PPU.h"


//Extern globals 
extern unsigned char   OAM_MEMORY[256];
extern unsigned char   PPU_MEMORY[16384];
extern unsigned char   NES_MEMORY[65536];





unsigned short Get_base_nametable_addr(void) {

	switch (NES_MEMORY[PPUCRTL] & 0x03) {

	case 0:
		return 0x2000;
		break;

	case 1:
		return 0x2400;
		break;

	case 2:
		return 0x2800;
		break;

	case 3:
		return 0x2C00;
		break;

	default:
		//Error
		break;
	}
}



void Clear_vblank(void) {

	NES_MEMORY[PPPUSTATUS] &= 0x7F;
}



void Set_vblank(void) {

	NES_MEMORY[PPPUSTATUS] |= 0x80;
}