#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>

FILE *mainScmFile, *mainTxtFile;
void *mainScm, *mainTxtOld, *mainTxtNew;

void cleanup(int retNum) {
    if (mainTxtFile)
		fclose(mainTxtFile);
	
	//free(mainScm);
	exit(retNum);
}

int main() {
    // Run a cleanup function in case the program is interrupted or terminates
	signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

    //////////////////////////////////////////////////////////////
    ////////////////////////////INIT//////////////////////////////
    //////////////////////////////////////////////////////////////

    // Need to open a main.scm with a corresponding main.txt.

    // Try automatically opening main.scm if it's present, otherwise prompt for it
	mainScmFile = fopen("main.scm", "rb");
	if (!mainScmFile) {
		puts("Enter the path to main.scm:");
		char* mainScmFilePath;
		scanf("%[^\n]", mainScmFilePath);
		mainScmFile = fopen(mainScmFilePath, "rb");
		if (!mainScmFile) {
			puts("Error opening main.scm.");
			cleanup(1);
		}
	}
	puts("main.scm loaded.");

    // Try automatically opening main.txt if it's present, otherwise prompt for it
	mainTxtFile = fopen("main.scm", "rb");
	if (!mainTxtFile) {
		puts("Enter the path to main.scm:");
		char* mainTxtFilePath;
		scanf("%[^\n]", mainTxtFilePath);
		mainTxtFile = fopen(mainTxtFilePath, "r");
		if (!mainTxtFile) {
			puts("Error opening main.txt");
			cleanup(1);
		}
	}
	puts("main.txt loaded.");

    // Get scm file length
    fseek(mainScmFile, 0, SEEK_END);
	uint32_t scmFileLength = ftell(mainScmFile);
	//printf("%d bytes long. ", fileLength);
    rewind(mainScmFile);

	// Get txt file length
    fseek(mainTxtFile, 0, SEEK_END);
	uint32_t txtFileLength = ftell(mainTxtFile);
	//printf("%d bytes long. ", fileLength);
    rewind(mainTxtFile);

    // Read the scm into a buffer
    mainScm = calloc(scmFileLength, 1);
    fread(mainScm, scmFileLength, 1, mainScmFile);

    // Read the txt into a buffer
	mainTxtOld = calloc(txtFileLength, 1);
	fread(mainTxtOld, txtFileLength, 1, mainTxtFile);

    // Create a second buffer for the changes.
    // This will be more than enough to work with, the program will keep track of the real length for finalization
    mainTxtNew = calloc(txtFileLength, 2); 

    //////////////////////////////////////////////////////////////
    /////////////////////////MAIN LOOP////////////////////////////
    //////////////////////////////////////////////////////////////
    uint32_t newTxtLength;
    // Basically, go line by line in the txt file until we find the first mission (with a global offset tag from Sanny)
    // Then we know where to start looking in the SCM buffer.
    // To know how much each instruction takes up, look ahead for the next opcode and search for it in the scm buffer.

    // Basically can dereference pairs of bytes as uint16_t,
    // and compare them with the txt opcode.
    // If a separate "valid opcode check is needed then do < 0x05FF (opcodes don't start with anything more than 05)
    
    // Then take the address of the next opcode's start and subtract the previous opcode's start from it.
    // Boom, add to local offset and insert on the line of the next opcode.

}