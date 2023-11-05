#include <stdio.h>
#include <Windows.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

// Is preceded by 51 00 (return) and starts a typical missionstart opcode (50 00 or A4 03) 
#define STARTOFMISSION *(bufferOffset-2) == 0x51 && *(bufferOffset-1) == 0x00 && ((*bufferOffset == 0x50 && *(bufferOffset+1) == 0x00) || (*bufferOffset == 0xA4 && *(bufferOffset+1) == 0x03))

// Check if matches either pattern 01 00 04 00 or 01 00 05 XX XX
#define WAIT (*(bufferIpCheckOffset-3) == 1 && *(bufferIpCheckOffset-2) == 0 && *(bufferIpCheckOffset-1) == 4 && *bufferIpCheckOffset == 0) || (*(bufferIpCheckOffset-4) == 1 && *(bufferIpCheckOffset-3) == 0 && *(bufferIpCheckOffset-2) == 5)

unsigned short missionSize[] =	// Will use the same mission IDs as in the SCM to identify missions. This may be used in the future for a more robust IP-finder program
{	
	0,	// Initial
	0,	// Intro
	0,	// An Old Friend
	0x32A8,	// The Party
	0x34F5,	// Back Alley Brawl
	0x240B,	// Jury Fury
	0x2DF6,	// Riot
	0x2560,	// Treacherous Swine
	0x27AB,	// Mall Shootout
	0x6D54,	// Guardian Angels
	0x7CB6,	// Sir, Yes Sir!
	0x7B22,	// All Hands On Deck!
	0x22A3,	// The Chase
	0x3CFC,	// Phnom Penh '86
	0x298B,	// The Fastest Boat
	0x326C,	// Supply & Demand
	0x3797,	// Rub Out
	0x3D81,	// Death Row
	0x2D03,	// Four Iron
	0x5D38,	// Demolition Man (SERG3 in scm)
	0x2970,	// Two Bit Hit (SERG2 in scm)
	0x65C0,	// No Escape?
	0x6BB4,	// The Shootist
	0x290E,	// The Driver
	0x7E98,	// The Job
	0x42EC,	// Gun Runner
	0x22D5,	// Boomshine Saigon
	0x2C55,	// Recruitment Drive
	0x1DF7,	// Dildo Dodo
	0x2B2E,	// Martha's Mug Shot
	0x2D9E,	// G-Spotlight
	0x1C7B,	// Shakedown
	0x3010,	// Bar Brawl
	0x33B5,	// Cop Land
	0x40B6,	// Spilling the Beans
	0x54E6,	// Hit the Courier
	0,	// Printworks buy
	0,	// Sunshine Autos buy
	0,	// Porn Studios buy
	0,	// Cherry Poppers buy
	0,	// Kaufman Cabs buy
	0,	// Malibu buy
	0,	// Boatyard buy
	0,	// Pole Position buy
	0,	// El Swanko Casa buy
	0,	// Links View buy
	0,	// Hyman Condo buy
	0,	// Ocean Heights buy
	0,	// 1102 Washington St buy
	0,	// Vice Point/Mall safehouse buy
	0,	// Skumole Shack buy
	0,	// Cap the Collector
	0,	// Keep Your Friends Close...
	0,	// Alloy Wheels of Steel
	0,	// Messing with the Man
	0,	// Hog Tied
	0,	// Stunt Boat Challenge
	0,	// Cannon Fodder
	0,	// Naval Engagement
	0,	// Trojan Voodoo
	0,	// Juju Scramble
	0,	// Bombs Away!
	0,	// Dirty Lickin's
	0,	// Love Juice
	0,	// Psycho Killer
	0,	// Publicity Tour
	0,	// Weapon Range
	0,	// Road Kill
	0,	// Waste the Wife
	0,	// Autocide
	0,	// Check Out at the Check In
	0,	// Loose Ends
	0,	// VIP
	0,	// Friendly Rivalry
	0,	// Cabmaggedon
	0,	// Taxi Driver
	0,	// Paramedic
	0,	// Firefighter
	0,	// Vigilante
	0,	// Hotring
	0,	// Bloodring
	0,	// Dirtring
	0,	// SSA Races
	0,	// Distribution
	0,	// Downtown Chopper Checkpoint
	0,	// Ocean Beach Chopper Checkpoint
	0,	// Vice Point Chopper Checkpoint
	0,	// Little Haiti Chopper Checkpoint
	0,	// Trial by Dirt
	0,	// Test Track
	0,	// PCJ Playground
	0,	// Cone Crazy
	0,	// Pizza Boy
	0,	// RC Raider
	0,	// RC Bandit
	0,	// RC Baron
	0	// Checkpoint Charlie
};	

FILE *mainScm;
unsigned char *fileBuffer, *bufferOffset, *bufferIpCheckOffset;
unsigned long fileLength;

void cleanup(int retNum);

int main()
{
	signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

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
	printf("%d bytes long.\n", fileLength);
	rewind(mainScm);

	if (fileLength != 1277288)
	{
		puts("File size does not match default VC JP size. Exiting.");
		cleanup(1);
	}

	fileBuffer = (char*)calloc(fileLength, 1);
	fread(fileBuffer, fileLength, 1, mainScm);
	rewind(mainScm);

	// Actual mission stuff starts line 1701 of the ASM barf in NP++, or offset 0x3E820 (start of An Old Friend)
	for (unsigned long offset = 0x3E820; offset < fileLength; offset++)
	{
		// Use offset for actual offsets in the file, and bufferOffset as a base for retrieving vals.
		bufferOffset = fileBuffer + offset;

		/*// Printing out data (for testing)
		if (!(offset % 16))
			printf("Offset 0x%06x -- ", offset);
		printf("%02x ", byteVal);
		Sleep(5);
		if (!((offset+1) % 16))
			printf("\n");
		*/

		if (STARTOFMISSION)
		{
			// Check start-of-taxi-to-taxi-pass offset from assumed end of mission/start of mission for waits
			for (unsigned short ipCheckOffset = 0x17FE; ipCheckOffset <= 0x1817; ipCheckOffset++)
			{
				bufferIpCheckOffset = bufferOffset + ipCheckOffset;
				if (WAIT)
					switch (offset+ipCheckOffset)	// Known cases
					{
						case 0x4077A:	// The Party one found by root
						case 0xA80CF:	// The Martha's Mug Shot one
						case 0x12972C:	// Leaks from TBD to Test Track
							break;
						default:
							printf("\nStart of mission at offset 0x%06x", offset);
							printf("\nWait found at 0x%06x\n", offset+ipCheckOffset);
							break;
					}
			}

			// Check taxi3-to-taxi-pass offset from assumed end of mission/start of mission for waits
			for (unsigned short ipCheckOffset = 0x2E08; ipCheckOffset <= 0x2E21; ipCheckOffset++)
			{
				bufferIpCheckOffset = bufferOffset + ipCheckOffset;
				if (WAIT)
					switch (offset+ipCheckOffset)
					{
						case 0x6AC04:	// Phnom Penh "you're on my turf asshole"
						case 0x93F2F:	// Leaks from The Driver to The Job
						case 0xA96D9:	// Leaks from martha's into G-S
						case 0xCF5E0:	// Leaks from biker 1 to biker 2-3
						case 0xFCECD:	// Leaks from assassin 2 to assassin 4
						case 0x130050:	// Pizza boy, "these need delivering" wav not loaded loop
							break;
						default:
							printf("\n(Taxi 3) Start of mission at offset 0x%06x", offset);
							printf("\n(Taxi 3) Wait found at 0x%06x\n", offset+ipCheckOffset);
							break;
					}
			}

			// Check taxi2-to-taxi-pass offset from assumed end of mission/start of mission for waits
			for (unsigned short ipCheckOffset = 0x34D8; ipCheckOffset <= 0x34F1; ipCheckOffset++)
			{
				bufferIpCheckOffset = bufferOffset + ipCheckOffset;
				if (WAIT)
					switch (offset+ipCheckOffset)
					{
						case 0x6EFD7:	// Leaks from fastest boat to S&D
						case 0x7194F:	// Leaks from S&D to rub out
						case 0x7C0E3:	// Leaks from four iron to two bit hit
						case 0x96F15:	// The Job, wait for the bank manager to be at coords -938.56 -351.5 16.8. 
										// The loop has a TIMERA > 2500 condition though so it's effectively a wait 2500. Pretty impossible in any case
						case 0xA7FAC:	// Leaks from dildo to martha's
						case 0xE6CA5:	// Leaks from juju to bombs away
						case 0x137B14:	// Leaks from rc baron to boatyard
							break;
						default:
							printf("\n(Taxi 2) Start of mission at offset 0x%06x", offset);
							printf("\n(Taxi 2) Wait found at 0x%06x\n", offset+ipCheckOffset);
							break;
					}
			}

			// Check taxi1-to-taxi-pass offset from assumed end of mission/start of mission for waits
			for (unsigned short ipCheckOffset = 0x44AF; ipCheckOffset <= 0x44C8; ipCheckOffset++)
			{
				bufferIpCheckOffset = bufferOffset + ipCheckOffset;
				if (WAIT)
					switch (offset+ipCheckOffset)
					{
						case 0x466DD:	// Leaks from lawyer 2 to lawyer 3
						case 0x49BCA:	// Leaks from lawyer 3 to lawyer 4
						case 0x6C29D:	// Leaks from phnom penh to fastest boat
						case 0x7933A:	// Leaks from death row to four iron
						case 0xA4060:	// Leaks from boomshine saigon to recruitment drive
						case 0xB868E:	// Leaks from pw1 to pw2
						case 0xD39A7:	// Leaks from biker 2 to stunt boat
							break;
						default:
							printf("\n(Taxi 1)Start of mission at offset 0x%06x", offset);
							printf("\n(Taxi 1) Wait found at 0x%06x\n", offset+ipCheckOffset);
							break;
					}
			}
		}

		// Continue checking for returns.
	}

    cleanup(0);
}

void cleanup(int retNum)
{
	if (&retNum == NULL)
		retNum = 1;

	if (mainScm)
		fclose(mainScm);
	
	/*for (ch = 0; ch < fileLength; ch++)
		free(&fileBuffer[ch]);*/
	free(fileBuffer);
	exit(retNum);
}
