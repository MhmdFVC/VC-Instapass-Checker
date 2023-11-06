#include <stdio.h>
#include <Windows.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

// Is preceded by 51 00 (return) and starts a typical missionstart opcode (50 00 or A4 03) 
//#define STARTOFMISSION *(bufferOffset-2) == 0x51 && *(bufferOffset-1) == 0x00 && ((*bufferOffset == 0x50 && *(bufferOffset+1) == 0x00) || (*bufferOffset == 0xA4 && *(bufferOffset+1) == 0x03))

// Check if matches either pattern 01 00 04 00 or 01 00 05 XX XX
#define WAIT (*(bufferIpCheckOffset-4) == 1 && *(bufferIpCheckOffset-3) == 0 && *(bufferIpCheckOffset-2) == 4 && *(bufferIpCheckOffset-1) == 0) || (*(bufferIpCheckOffset-5) == 1 && *(bufferIpCheckOffset-4) == 0 && *(bufferIpCheckOffset-3) == 5)

typedef struct
{
	unsigned short size;
	unsigned int offset;
	unsigned char name[35];
} Mission;

unsigned int offset = 0x3AF5D, offsetTemp;	// Use initially as "initial mission" offset. This is for Intro, the program main loop will jump right to An Old Friend though.
FILE *mainScm;
unsigned char *fileBuffer, *bufferIpCheckOffset, missionSelect, offsetType, numMissionsBack = 0, matchFound;
unsigned short ipCheckOffsetStart, ipCheckOffsetEnd;
unsigned long fileLength;

Mission mission[97] =	// Will use the same order as the mission IDs as in the SCM to identify missions
{	
	{0x777A, 0, "Initial"},
	{0x38C3, 0x3AF5D, "Intro"},
	{0x754, 0x03e820, "An Old Friend"},
	{0x32A8, 0x03ef74, "The Party"},
	{0x34F5, 0x04221c, "Back Alley Brawl"},
	{0x240B, 0x045711, "Jury Fury"},
	{0x2DF6, 0x047b1c, "Riot"},
	{0x2560, 0x04a912, "Treacherous Swine"},
	{0x27AB, 0x04ce72, "Mall Shootout"},
	{0x6D54, 0x04f61d, "Guardian Angels"},
	{0x7CB6, 0x056371, "Sir, Yes Sir!"},
	{0x7B22, 0x05e027, "All Hands On Deck!"},
	{0x22A3, 0x065b49, "The Chase"},
	{0x3CFC, 0x067dec, "Phnom Penh '86"},
	{0x298B, 0x06bae8, "The Fastest Boat"},
	{0x326C, 0x06bae8, "Supply & Demand"},
	{0x3797, 0x0716df, "Rub Out"},
	{0x3D81, 0x074e76, "Death Row"},
	{0x2D03, 0x078bf7, "Four Iron"},
	{0x5D38, 0x07b8fa, "Demolition Man"},
	{0x2970, 0x081632, "Two Bit Hit"},
	{0x65C0, 0x083fa2, "No Escape?"},
	{0x6BB4, 0x08a562, "The Shootist"},
	{0x290E, 0x091116, "The Driver"},
	{0x7E98, 0x093a24, "The Job"},
	{0x42EC, 0x09b8bc, "Gun Runner"},
	{0x22D5, 0x09fba8, "Boomshine Saigon"},
	{0x2C55, 0x0a1e7d, "Recruitment Drive"},
	{0x1DF7, 0x0a4ad2, "Dildo Dodo"},
	{0x2B2E, 0x0a68c9, "Martha's Mug Shot"},
	{0x2D9E, 0x0a93f7, "G-Spotlight"},
	{0x1C7B, 0x0ac195, "Shakedown"},
	{0x3010, 0x0ade10, "Bar Brawl"},
	{0x33B5, 0x0b0e20, "Cop Land"},
	{0x40B6, 0x0b41d5, "Spilling the Beans"},
	{0x54E6, 0x0b828b, "Hit the Courier"},
	{0x2E2,	0x0bd771, "Printworks buy"},
	{0x72E, 0x0bda53, "Sunshine Autos buy"},
	{0x345, 0x0be181, "Porn Studios buy"},
	{0x90C,	0x0be4c6, "Cherry Poppers buy"},
	{0x8FE,	0x0bedd2, "Kaufman Cabs buy"},
	{0x29F,	0x0bf6d0, "Malibu buy"},
	{0x687,	0x0bf96f, "Boatyard buy"},
	{0x26E,	0x0bfff6, "Pole Position buy"},
	{0x263,	0x0c0264, "El Swanko Casa buy"},
	{0x263,	0x0c04c7, "Links View buy"},
	{0x287,	0x0c072a, "Hyman Condo buy"},
	{0x263,	0x0c09b1, "Ocean Heights buy"},
	{0x215,	0x0c0c14, "1102 Washington St buy"},
	{0x215,	0x0c0e29, "Vice Point/Mall safehouse buy"},
	{0x215,	0x0c103e, "Skumole Shack buy"},
	{0x4913, 0x0c1253, "Cap the Collector"},
	{0x6C5C, 0x0c5b66, "Keep Your Friends Close..."},
	{0x2D24, 0x0cc7c2, "Alloy Wheels of Steel"},
	{0x857, 0x0cf4e6, "Messing with the Man"},
	{0x32D4, 0x0cfd3d, "Hog Tied"},
	{0x2A73, 0x0d3011, "Stunt Boat Challenge"},
	{0x5960, 0x0d5a84, "Cannon Fodder"},
	{0x2DC2, 0x0db3e4, "Naval Engagement"},
	{0x5610, 0x0de1a6, "Trojan Voodoo"},
	{0x316B, 0x0e37b6, "Juju Scramble"},
	{0x45C9, 0x0e6921, "Bombs Away!"},
	{0x4583, 0x0eaeea, "Dirty Lickin's"},
	{0x2331, 0x0ef46d, "Love Juice"},
	{0x2F48, 0x0f179e, "Psycho Killer"},
	{0x38E0, 0x0f46e6, "Publicity Tour"},
	{0x194D, 0x0f7fc6, "Weapon Range"},
	{0x7A1, 0x0f9913, "Road Kill"},
	{0x10AF, 0x0fa0b4, "Waste the Wife"},
	{0x1AAD, 0x0fb163, "Autocide"},
	{0x21C8, 0x0fcc10, "Check Out at the Check In"},
	{0x31B2, 0x0fedd8, "Loose Ends"},
	{0xFD7, 0x101f8a, "V.I.P."},
	{0x6D0, 0x102f61, "Friendly Rivalry"},
	{0x160A, 0x103631, "Cabmaggedon"},
	{0x1BC9, 0x104c3b, "Taxi Driver"},
	{0x216F, 0x106804, "Paramedic"},
	{0x213F, 0x108973, "Firefighter"},
	{0x42A7, 0x10aab2, "Vigilante"},
	{0x4AFA, 0x10ed59, "Hotring"},
	{0x3E0D, 0x113853, "Bloodring"},
	{0x42CA, 0x117660, "Dirtring"},
	{0x7DB1, 0x11b92a, "SSA Races"},
	{0x1114, 0x1236db, "Distribution"},
	{0xEB3, 0x1247ef, "Downtown Chopper Checkpoint"},
	{0xD4B, 0x1256a2, "Ocean Beach Chopper Checkpoint"},
	{0xD4B, 0x1263ed, "Vice Point Chopper Checkpoint"},
	{0xDE1, 0x127138, "Little Haiti Chopper Checkpoint"},
	{0x1350, 0x127f19, "Trial by Dirt"},
	{0x11CB, 0x129269, "Test Track"},
	{0x1A95, 0x12a434, "PCJ Playground"},
	{0x1368, 0x12bec9, "Cone Crazy"},
	{0x387D, 0x12d231, "Pizza Boy"},
	{0x192C, 0x130aae, "RC Raider"},
	{0x225D, 0x1323da, "RC Bandit"},
	{0x1A2B, 0x134637, "RC Baron"},
	{0x1D06, 0x136062, "Checkpoint Charlie"}
};	

void cleanup(int retNum);

int main()
{
	signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////READING IN MAIN.SCM////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	mainScm = fopen("main.scm", "rb");
	if (!mainScm)
	{
		char mainScmFilePath[0xFF];
		puts("Enter the path to main.scm:");
		scanf("%[^\n]", &mainScmFilePath);
		mainScm = fopen(mainScmFilePath, "rb");
		if (!mainScm)
		{
			puts("Error opening main.scm.");
			cleanup(1);
		}
	}
	puts("File opened.");

	// Get file size
	fseek(mainScm, 0, SEEK_END);
	fileLength = ftell(mainScm);
	//printf("%d bytes long. ", fileLength);
	rewind(mainScm);

	switch (fileLength)
	{
	 case 1277288:
		puts("Haitian-friendly version.");
		break;
	case 1269133:
		puts("Original version. Currently unsupported. Exiting.");
		cleanup(1);
	 default:
		puts("Input size does not match any supported VC versions' SCM size. Exiting.");
		cleanup(1);
	}

	fileBuffer = (char*)calloc(fileLength, 1);
	fread(fileBuffer, fileLength, 1, mainScm);
	rewind(mainScm);

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////PROVIDING INFO AND TAKING INPUT FOR IP CHECK PARAMETERS/////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Print out all of the (actual) missions and their mission IDs
	for (unsigned char missionIndex = 2; missionIndex < 97; missionIndex++)
	{
		offsetTemp += mission[missionIndex-1].size;
		printf("%d - %s (global offset 0x%06x)\n", missionIndex, mission[missionIndex].name, offsetTemp);
	}
	puts("");

	// Mission selection
	printf("Please enter the ID of the mission you would like to check for instapasses (2-96): ");
	scanf("%d", &missionSelect);
	printf("You selected %s\n", mission[missionSelect].name);
	if (missionSelect < 2 || missionSelect > 96)
	{
		puts("Invalid value. Exiting.");
		cleanup(1);
	}
	
	// Prompt for global or local offset input for IP check range
	printf("Please now enter the range of mission offsets you would like to check for instapasses for. Enter global or local offset? g/l: ");
	getchar();	// To clear dangling newline
	offsetType = getchar();
	if (!(offsetType == 'g' || offsetType == 'l'))
	{
		puts("You entered an invalid offset type. Exiting.");
		cleanup(1);
	}

	// Get IP check address range
	printf("Enter the ");
	if (offsetType == 'g')
		printf("global offset range start in hexadecimal (0-137D60): ");
	else printf("local offset range start in hexadecimal (0-FFFF): ");
	scanf("%x", &ipCheckOffsetStart);
	if ((offsetType == 'g' && ipCheckOffsetStart > 0x137D60))	// No handling for if it's local because higher than FFFF and it overflows anyway.
	{
		puts("You entered an invalid value. Exiting.");
		cleanup(1);
	}
	printf("Enter the ");
	if (offsetType == 'g')
		printf("global offset range end in hexadecimal (0-137D60): ");
	else printf("local offset range end in hexadecimal (0-FFFF): ");
	scanf("%x", &ipCheckOffsetEnd);
	if ((offsetType == 'g' && ipCheckOffsetEnd > 0x137D60))	// No handling for if it's local because higher than FFFF and it overflows anyway.
	{
		puts("You entered an invalid value. Exiting.");
		cleanup(1);
	}
	if (ipCheckOffsetStart > ipCheckOffsetEnd)
	{
		puts("You entered an end smaller than the start of the range. Exiting.");
		cleanup(1);
	}
	if (offsetType == 'g')
	{
		ipCheckOffsetStart -= mission[missionSelect].offset;
		ipCheckOffsetEnd -= mission[missionSelect].offset;
	}
	
	// Prompt for checking the mission n missions prior to the mission-to-be-instapassed in the script
	printf("Enter number of missions back in the script you would like to test. For example, to test instapasses by starting the previous mission, enter 1. (0 if N/A) ");
	scanf("%d", &numMissionsBack);
	if (numMissionsBack > missionSelect || numMissionsBack > 96)
	{
		puts("You entered an invalid value. Exiting.");
		cleanup(1);
	}
	if (numMissionsBack) printf("Testing instapasses for %s when starting %s\n", mission[missionSelect].name, mission[missionSelect-numMissionsBack].name);
	// Increasing IP check offset range by the size of the additional missions
	if (numMissionsBack)
		for (unsigned char numMissionsBackCount = 1; numMissionsBackCount <= numMissionsBack; numMissionsBackCount++)
		{
			ipCheckOffsetStart += mission[missionSelect-numMissionsBackCount].size;
			ipCheckOffsetEnd += mission[missionSelect-numMissionsBackCount].size;
		}
	printf("Checking local offset range 0x%x - 0x%x\n", ipCheckOffsetStart, ipCheckOffsetEnd);

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////MAIN LOOP//////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Loop through all (actual) missions
	for (unsigned char missionIndex = 2; missionIndex <= 96; missionIndex++)
	{
		offset += mission[missionIndex-1].size; // Jump to the current mission's offset
		
		matchFound = 0;	// Reset flag indicating that at least one match has been found for this mission already
		if (mission[missionIndex].size < ipCheckOffsetStart)
			continue;	// Skip missions that are too short to possibly work
		if (missionIndex == missionSelect)
			continue;	// Skip checking the mission-to-be-instapassed because that would just be duping 
		// Check each IP check offset within the provided range for waits
		for (unsigned short ipCheckOffset = ipCheckOffsetStart; ipCheckOffset <= ipCheckOffsetEnd; ipCheckOffset++)
		{
			bufferIpCheckOffset = fileBuffer + offset + ipCheckOffset;	// Address of file buffer object + global offset of mission being checked + local offset within instapass check range
			if (WAIT)	// Wait command found
			{
				if (!matchFound)	// If no match had been previously found for this mission already
				{
					printf("\n%s: (0x%06x)", mission[missionIndex].name, offset);	// Prints the mission name only on the first match of the mission
					matchFound = 1;
				}
				printf("\n\tWait found at global offset 0x%06x, or mission offset 0x%x", offset+ipCheckOffset, ipCheckOffset);	// Prints findings
			}
		} // Done checking current mission for waits
	} // All applicable missions checked
    cleanup(0);	// End prog successfully
}

void cleanup(int retNum)
{
	if (&retNum == NULL)
		retNum = 1;

	if (mainScm)
		fclose(mainScm);
	
	free(fileBuffer);
	exit(retNum);
}

/*// Printing out data (for testing)
		if (!(offset % 16))
			printf("Offset 0x%06x -- ", offset);
		printf("%02x ", byteVal);
		Sleep(5);
		if (!((offset+1) % 16))
			printf("\n");
		*/