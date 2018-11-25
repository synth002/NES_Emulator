



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

unsigned char PPU_MEMORY_SPACE[15359];	//15360 Bytes
unsigned char OBJECT_ATTRIBUTE_MEMORY[63][3];