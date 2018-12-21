
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NES_PPU.h"



/*
	**** Screen resolution: 256 x 240 ****
	NOTE: For NTSC NES, the actual area displayed on screen is 256x224

	**** Three PPU clocks for every CPU clock event ****

	PPU renders 262 scanlines per frame, each scanline lasts for 341 clock cycles,
	with each clock cycle producing one pixel.



	---- Line-by-Line Timing ----

	Pre-render scanline (-1/261)



	--- PPU Memory Map ----

	---------------------
	0x0000-0x0FFF	(4096 Bytes)	Pattern table 0
	0x1000-0x1FFF	(4096 Bytes)	Pattern Table 1
	---------------------
	0x2000-0x23BF	(n Bytes)		Nametable 0
	0x23C0-0x23FF   (n Bytes)		Attribute Table 0
	---------------------
	0x2400-0x27BF	(n Bytes)		Nametable 1
	0x27C0-0x27FF   (n Bytes)		Attribute Table 1
	---------------------
	0x2800-0x2BBF	(n Bytes)		Nametable 2
	0x2BC0-0x2BFF   (n Bytes)		Attribute Table 2
	---------------------
	0x2C00-0x2FBF	(n Bytes)		Nametable 3
	0x2FC0-0x2FFF	(n Bytes)		Attribute Table 3
	---------------------
	0x3000-0x3EFF	(n Bytes)		Mirrors of 0x2000-0x2EFF
	---------------------
	0x3F00-0x3F0F	(n Bytes)		Background Palettes
	0x3F10-0x3F1F   (n Bytes)		Sprite Palettes
	---------------------
	0x3F20-0x3FFF   (n Bytes)		Mirrors of 0x3F00-0x3F1F
	---------------------
	0x4000-0xFFFF	(n Bytes)		Mirrors of 0x0000-0x3FFF
	---------------------
*/




//Local globals 
unsigned char        OAM_MEMORY[256]	= { 0 };
unsigned char		 PPU_MEMORY[16384]	= { 0 };

//Extern globals
extern unsigned char NES_MEMORY[65536];




//******************************************************
//********** Init PPU runtime attributes ***************
void Init_ppu_attributes(ppu_emu_dat *ppu_emu_data) {

	ppu_emu_data->cycle = 0;
	ppu_emu_data->state = pre_render;
}
//******************************************************
//******************************************************




//****************************************************************
//*************** NES PPU STATE MACHINE **************************
void PPU_Cycle(unsigned char reset) {

	static ppu_emu_dat ppu_emu_data;

	if (reset) {
		Init_ppu_attributes(&ppu_emu_data);
	}

	switch (1) {

	case pre_render:
		Pre_render(&ppu_emu_data);
		break;

	case render:
		break;

	case sprite_tiles_for_next:
		break;

	case two_tiles_for_next:
		break;

	case two_byte_fetches:
		break;

	case post_render:
		break;

	case Vblank: 

		break;

	default:
		break;

	}

}
//****************************************************************
//****************************************************************


void Pre_render(ppu_emu_dat * ppu_emu_data) {

	static unsigned short nametable_addr;

	switch (ppu_emu_data->cycle) {

	case 1:
		nametable_addr = Get_base_nametable_addr();
		break;

	case 2:
		
		break;


	default:
		//Error
		break;

	}

	ppu_emu_data->cycle++;

}