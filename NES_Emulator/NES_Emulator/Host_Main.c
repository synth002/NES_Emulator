#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NES_EMU\NES.h"


FILE *ROM_FP;
FILE *CPU_LOG_TXT;
FILE *CPU_LOG_CSV;




void Grab_rom_data(unsigned char *NES_MEMORY, unsigned int write_address, unsigned int num_bytes, unsigned int read_address) {

	//This function is called by the emulator
	//to request ROM data from the host
	unsigned int a;

	/*
	//***** For testing purposes only! ******************
	printf("****************************\n");
	printf("write_address :  0x%.4X\n", write_address);
	printf("num_bytes : %u\n", num_bytes);
	printf("read_address : 0x%.4X\n", read_address);
	printf("****************************\n");
	//***************************************************
	*/

	fseek(ROM_FP, read_address, SEEK_SET);

	for (a = 0; a <= num_bytes - 1; a++ ) {
		NES_MEMORY[write_address] = fgetc(ROM_FP);
		//***** For testing purposes only! ******************
		//printf("write to : 0x%.4X,  Value of : 0x%.2X\n", write_address, NES_MEMORY[write_address]);
		//***************************************************
		write_address++;
	}
}




#ifdef LOGGING_ENABLED
void Update_txt_log(char *log_string) {

	//This function is called by the emulator 
	//to update verbsose txt logging, if enabled
	static unsigned short a = 0;
	if (a < 600) a++;

	fputs(log_string, CPU_LOG_TXT);
	if(a == 650) {
		fclose(CPU_LOG_TXT);
	}
}
#endif




#ifdef LOGGING_ENABLED
void Update_csv_log(char *log_string) {

	//This function is called by the emulator 
	//to update csv logging, if enabled
	static unsigned short a = 0;
	if (a < 600) a++;

	fputs(log_string, CPU_LOG_CSV);
	if (a == 650) {
		fclose(CPU_LOG_CSV);
	}
}
#endif







int main(void) {


	char ROM_header[0x0F];


	//Open NES ROM file for reading
	ROM_FP = fopen("nestest.nes", "rb");	//File must be opened in binary read mode!!!!
	if (ROM_FP == NULL) {
		printf("Error opening NES ROM file!");
	}
	
	
	//Get ROM header data
	for (unsigned char a = 0; a <= 0x0F - 1; a++) {
		ROM_header[a] = fgetc(ROM_FP);
		if( a == 2) 	fseek(ROM_FP, 4, SEEK_SET);
	}



	//Setup emulator (Check vaid header file)
	if ( Setup_emulator(ROM_header) ) { 
		printf("valid ROM!"); 
	}
	else { 
		printf("invalid ROM!"); 
	}


#ifdef LOGGING_ENABLED
	CPU_LOG_TXT = fopen("CPU_LOG_TXT.txt", "w");
	if (CPU_LOG_TXT == NULL) {
		printf("Error creating log file");
	}

	CPU_LOG_CSV = fopen("CPU_LOG_CSV.csv", "w");
	if (CPU_LOG_CSV == NULL) {
		printf("Error creating log file");
	}
#endif
	


	while (1) { 

		
		Emulator_action_tick();
	}

	fclose(ROM_FP);
	fclose(CPU_LOG_TXT);

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