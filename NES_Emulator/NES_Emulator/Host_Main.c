#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NES_EMU\NES.h"

#define NES_HEADER_LEN  0x0F
FILE *ROM_FP;




void Grab_rom_data(unsigned char *NES_MEMORY, unsigned int bytes, unsigned char trainer) {

	//This function is called by the emulator
	//to request ROM data from the host


}




int main(void) {


	char ROM_header[NES_HEADER_LEN];


	//Open NES ROM file for reading
	ROM_FP = fopen("nestest.nes", "r");
	if (ROM_FP == NULL) {
		printf("Error opening NES ROM file!");
	}
	
	
	//Get ROM header data
	for (unsigned char a = 0; a <= NES_HEADER_LEN - 1; a++) {
		ROM_header[a] = fgetc(ROM_FP);
		if( a == 2) 	fseek(ROM_FP, 4, SEEK_SET);
	}


	fclose(ROM_FP);


	//Setup emulator (Check vaid header file)
	if ( Setup_emulator(ROM_header) ) { 
		printf("valid ROM!"); 
	}
	else { 
		printf("invalid ROM!"); 
	}



	while (1) { }


	return 0;
}







//***************************************************		//Consider integrating this into emulator!
//*************** Error handling ********************
char* Check_error_code(unsigned short error_code) {

	//This function is used to lookup any error state
	//that have arisen. 


	switch (error_code) {

	case 0 :
		//error state 0
		break;

	case 1 :
		//error state 1
		break;

	case 2 :
		//error state 2
		break;

	default :
		//error
		break;
			
	}

	return 0;
}
//***************************************************
//***************************************************