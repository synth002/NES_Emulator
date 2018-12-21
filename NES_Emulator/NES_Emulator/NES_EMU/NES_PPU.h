#pragma once




//PPU CPU ACCESSABLE REGISTERS  //Access mode
#define PPUCRTL		0x2000		//Write
#define PPUMASK		0x2001		//Write
#define PPPUSTATUS	0x2002		//Read
#define OAMADDR		0x2003		//Write
#define OAMDATA		0x2004		//Read/Write
#define PPUSCROLL   0x2005		//Write twice
#define PPUADDR		0x2006		//Write twice
#define PPUDATA		0x2007		//Read/Write
#define OAMDMA		0x4014		//Write




typedef enum {

	pre_render,
	render,
	sprite_tiles_for_next,
	two_tiles_for_next,
	two_byte_fetches,
	post_render,
	Vblank

} PPU_states;




typedef struct {

	unsigned char state;
	unsigned short cycle;


} ppu_emu_dat;




//**** Function prototypes
extern unsigned short Get_base_nametable_addr(void);

extern void Set_vblank(void);
extern void Clear_vblank(void);
