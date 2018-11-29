#pragma once



/*
	This should be the only header file required by the host system
	in order to run the NES emulator!
*/



//Should be called by host at startup or system reset
extern unsigned char Setup_emulator(char *ROM_header);


//This function is called by the emulator and handled by the host
extern void Grab_rom_data(unsigned char *NES_MEMORY, unsigned int write_adress, unsigned int num_bytes, unsigned int read_address);

//Should be called by host at fixed timing intervals
extern void Emulator_action_tick(void);