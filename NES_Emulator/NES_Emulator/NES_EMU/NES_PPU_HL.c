



/*
	The NTSC video signal is made up of 262 scanlines, and 20 of those are spent in vblank state.
	After the program has received an NMI, it has about 2270 cycles to update the palette, sprites, 
	and nametables as necessary before rendering begins.
*/

/*
	On NTSC systems, the PPU divides the master clock by 4 while the CPU uses the master clock divided by 12. 
	Since both clocks are fed off the same master clock, this means that there are exactly three PPU ticks per CPU cycle,
	with no drifting over time(though the clock alignment might vary depending on when you press the Reset button).
*/


//Resolution 256 (H) x 240 (V)


extern unsigned char NES_MEMORY[65536];

unsigned char NES_PPU_OUTPUT[240][256];


/*
	PPU Memory Map 

	$0000-$0FFF	(4096 Bytes)	Pattern table 0
	$1000-$1FFF	(4096 Bytes)	Pattern Table 1
	$2000-$23FF	(1024 Bytes)	Nametable 0
	$2400-$27FF	(1024 Bytes)	Nametable 1
	$2800-$2BFF	(1024 Bytes)	Nametable 2
	$2C00-$2FFF	(1024 Bytes)	Nametable 3
	$3000-$3EFF	(3840 Bytes)	Mirrors of $2000-$2EFF
	$3F00-$3F1F	(32 Bytes)		Palette RAM indexes
	$3F20-$3FFF	(224 Bytes)		Mirrors of $3F00-$3F1F

	Total : 15360 Bytes
*/



void PPU_Cycle(void) {




}