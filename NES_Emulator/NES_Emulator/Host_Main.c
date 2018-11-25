#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NES_EMU\NES.h"


#define NES_HEADER_LEN  0x0F







int main(void) {

	//Function place holders
	unsigned char a;


	//For error handling
	//unsigned short error_code;
	//char *error_string;



	FILE *ROM_FP;
	char ROM_header[NES_HEADER_LEN];



	//Open NES ROM file 
	ROM_FP = fopen("Super_Mario_Bros.nes", "r");
	if (ROM_FP == NULL) {
		printf("Error opening NES ROM file!");
	}
	

	
	//Get ROM header data
	for (a = 0; a <= NES_HEADER_LEN - 1; a++) {
		ROM_header[a] = fgetc(ROM_FP);
		if( a == 2) 	fseek(ROM_FP, 4, SEEK_SET);
	}

	fclose(ROM_FP);


	//Setup emulator (Check vaid header file)
	a = Setup_emulator(ROM_header);
	if (a == 1) { printf("valid ROM!"); }
	else { printf("invalid ROM!"); }



	
	
	while (1) {

	};


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