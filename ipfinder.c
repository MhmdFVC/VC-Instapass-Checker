#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include "include/md5.h"

void cleanup(int retNum);
int isWait();

typedef struct {
	uint16_t size;
	uint32_t offset;
	uint8_t name[36];
} Mission;

	

FILE* mainScmFile;
void *mainScm, *mainScmCursor;
int matchFound, waitType;
uint16_t numMissionsBack = 0;
uint32_t fileLength, ipCheckOffsetStart, ipCheckOffsetEnd, jumpedMissionsSize=0;

static const uint8_t md5_jp[] = { 0xF0, 0xE8, 0xD6, 0x48, 0xFD, 0x9E, 0x5C, 0xA1, 0x8E, 0xFF, 0xE6, 0x2B, 0x2A, 0x08, 0x78, 0x9E };
enum {
	INITIAL, INTRO, AN_OLD_FRIEND, THE_PARTY, BACK_ALLEY_BRAWL, JURY_FURY, RIOT, TSWINE,
	MALL_SHOOTOUT, GUARDIAN_ANGELS, SIR_YES_SIR, AHOD, THE_CHASE, PP86, THE_FASTEST_BOAT, SUPPLY_AND_DEMAND,
	RUB_OUT, DEATH_ROW, FOUR_IRON, DEMOLITION_MAN, TWO_BIT_HIT, NO_ESCAPE, THE_SHOOTIST, THE_DRIVER,
	THE_JOB, GUN_RUNNER, BOOMSHINE_SAIGON, RECRUITMENT_DRIVE, DILDO_DODO, MARTHAS_MUG_SHOT, GS, SHAKEDOWN,
	BAR_BRAWL, COP_LAND, STB, HTC, PRINTWORKS_BUY, SSA_BUY, FILMSTUDIO_BUY, CP_BUY,
	KAUFMAN_BUY, MALIBU_BUY, BOATYARD_BUY, PP_BUY, SWANKO_BUY, LINKSVIEW_BUY, HYMAN_BUY, OCEANHGTS_BUY,
	WASHST_BUY, MALLSH_BUY, SKUMOLE_BUY, CTC, KYFC, AWOS, MWTM, HOG_TIED,
	STUNT_BOAT_CHALLENGE, CANNON_FODDER, NAVAL_ENGAGEMENT, TROJAN_VOODOO, JUJU_SCRAMBLE, BOMBS_AWAY, DIRTY_LICKINS, LOVE_JUICE,
	PSYCHO_KILLER, PUBLICITY_TOUR, WEAPON_RANGE, ROAD_KILL, WASTE_THE_WIFE, AUTOCIDE, COATCI, LOOSE_ENDS,
	VIP, FRIENDLY_RIVALRY, CABMAGGEDON, TAXI, PARAMEDIC, FIREFIGHTER, VIGILANTE, HOTRING,
	BLOODRING, DIRTRING, RACES, DISTRIBUTION, DOWNTOWN_CC, OCEANBEACH_CC, VICEPOINT_CC, LITTLEHAITI_CC,
	TRIAL_BY_DIRT, TEST_TRACK, PCJ_PLAYGROUND, CONE_CRAZY, PIZZA_BOY, RC_RAIDER, RC_BANDIT, RC_BARON, CHECKPOINT_CHARLIE
};

static const Mission mission[] =	// JP. Will use the same order as the mission IDs as in the SCM to identify missions
{	
	{0x777A,	0, "Initial"},
	{0x38C3,	0x3AF5D, "Intro"},
	{0x754,		0x3E820, "An Old Friend"},
	{0x32A8,	0x3EF74, "The Party"},
	{0x34F5,	0x4221C, "Back Alley Brawl"},
	{0x240B,	0x45711, "Jury Fury"},
	{0x2DF6,	0x47B1C, "Riot"},
	{0x2560,	0x4A912, "Treacherous Swine"},
	{0x27AB,	0x4CE72, "Mall Shootout"},
	{0x6D54,	0x4F61D, "Guardian Angels"},
	{0x7CB6,	0x56371, "Sir, Yes Sir!"},
	{0x7B22,	0x5E027, "All Hands On Deck!"},
	{0x22A3,	0x65B49, "The Chase"},
	{0x3CFC,	0x67DEC, "Phnom Penh '86"},
	{0x298B,	0x6BAE8, "The Fastest Boat"},
	{0x326C,	0x6E473, "Supply & Demand"},
	{0x3797,	0x716DF, "Rub Out"},
	{0x3D81,	0x74E76, "Death Row"},
	{0x2D03,	0x78BF7, "Four Iron"},
	{0x5D38,	0x7B8FA, "Demolition Man"},
	{0x2970,	0x81632, "Two Bit Hit"},
	{0x65C0,	0x83FA2, "No Escape?"},
	{0x6BB4,	0x8A562, "The Shootist"},
	{0x290E,	0x91116, "The Driver"},
	{0x7E98,	0x93A24, "The Job"},
	{0x42EC,	0x9B8BC, "Gun Runner"},
	{0x22D5,	0x9FBA8, "Boomshine Saigon"},
	{0x2C55,	0xA1E7E, "Recruitment Drive"},
	{0x1DF7,	0xA4AD2, "Dildo Dodo"},
	{0x2B2E,	0xA68C9, "Martha's Mug Shot"},
	{0x2D9E,	0xA93F7, "G-Spotlight"},
	{0x1C7B,	0xAC195, "Shakedown"},
	{0x3010,	0xADE10, "Bar Brawl"},
	{0x33B5,	0xB0E20, "Cop Land"},
	{0x40B6,	0xB41D5, "Spilling the Beans"},
	{0x54E6,	0xB828B, "Hit the Courier"},
	{0x2E2,		0xBD771, "Printworks buy"},
	{0x72E,		0xBDA53, "Sunshine Autos buy"},
	{0x345,		0xBE181, "Porn Studios buy"},
	{0x90C,		0xBE4C6, "Cherry Poppers buy"},
	{0x8FE,		0xBEDD2, "Kaufman Cabs buy"},
	{0x29F,		0xBF6D0, "Malibu buy"},
	{0x687,		0xBF96F, "Boatyard buy"},
	{0x26E,		0xBFFF6, "Pole Position buy"},
	{0x263,		0xC0264, "El Swanko Casa buy"},
	{0x263,		0xC04C7, "Links View buy"},
	{0x287,		0xC072A, "Hyman Condo buy"},
	{0x263,		0xC09B1, "Ocean Heights buy"},
	{0x215,		0xC0C14, "1102 Washington St buy"},
	{0x215,		0xC0E29, "Vice Point/Mall safehouse buy"},
	{0x215,		0xC103E, "Skumole Shack buy"},
	{0x4913,	0xC1253, "Cap the Collector"},
	{0x6C5C,	0xC5B66, "Keep Your Friends Close..."},
	{0x2D24,	0xCC7C2, "Alloy Wheels of Steel"},
	{0x857,		0xCF4E6, "Messing with the Man"},
	{0x32D4,	0xCFD3D, "Hog Tied"},
	{0x2A73,	0xD3011, "Stunt Boat Challenge"},
	{0x5960,	0xD5A84, "Cannon Fodder"},
	{0x2DC2,	0xDB3E4, "Naval Engagement"},
	{0x5610,	0xDE1A6, "Trojan Voodoo"},
	{0x316B,	0xE37B6, "Juju Scramble"},
	{0x45C9,	0xE6921, "Bombs Away!"},
	{0x4583,	0xEAEEA, "Dirty Lickin's"},
	{0x2331,	0xEF46D, "Love Juice"},
	{0x2F48,	0xF179E, "Psycho Killer"},
	{0x38E0,	0xF46E6, "Publicity Tour"},
	{0x194D,	0xF7FC6, "Weapon Range"},
	{0x7A1,		0xF9913, "Road Kill"},
	{0x10AF,	0xFA0B4, "Waste the Wife"},
	{0x1AAD,	0xFB163, "Autocide"},
	{0x21C8,	0xFCC10, "Check Out at the Check In"},
	{0x31B2,	0xFEDD8, "Loose Ends"},
	{0xFD7,		0x101F8A, "V.I.P."},
	{0x6D0,		0x102F61, "Friendly Rivalry"},
	{0x160A,	0x103631, "Cabmaggedon"},
	{0x1BC9,	0x104C3B, "Taxi Driver"},
	{0x216F,	0x106804, "Paramedic"},
	{0x213F,	0x108973, "Firefighter"},
	{0x42A7,	0x10AAB2, "Vigilante"},
	{0x4AFA,	0x10ED59, "Hotring"},
	{0x3E0D,	0x113853, "Bloodring"},
	{0x42CA,	0x117660, "Dirtring"},
	{0x7DB1,	0x11B92A, "SSA Races"},
	{0x1114,	0x1236DB, "Distribution"},
	{0xEB3,		0x1247EF, "Downtown Chopper Checkpoint"},
	{0xD4B,		0x1256a2, "Ocean Beach Chopper Checkpoint"},
	{0xD4B,		0x1263ED, "Vice Point Chopper Checkpoint"},
	{0xDE1,		0x127138, "Little Haiti Chopper Checkpoint"},
	{0x1350,	0x127F19, "Trial by Dirt"},
	{0x11CB,	0x129269, "Test Track"},
	{0x1A95,	0x12A434, "PCJ Playground"},
	{0x1368,	0x12BEC9, "Cone Crazy"},
	{0x387D,	0x12D231, "Pizza Boy"},
	{0x192C,	0x130AAE, "RC Raider"},
	{0x225D,	0x1323DA, "RC Bandit"},
	{0x1A2B,	0x134637, "RC Baron"},
	{0x1D06,	0x136062, "Checkpoint Charlie"}
};	

// Use initially as "initial mission" offset. This is for Intro; the program main loop will jump right to An Old Friend though.
uint32_t missionSelect;

int main() {
	signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////READING IN MAIN.SCM////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	// Check file hash
	uint8_t md5[16];
	md5File(mainScmFile, md5);
	printf("MD5: ");
	for (int i = 0; i < sizeof(md5); printf("%02X", md5[i++]));
	puts("");

	if (!memcmp(md5, md5_jp, sizeof(md5))) {
		puts("Haitian-friendly version.");
		fileLength = 1277288;
	} else {
		puts("Hash does not match any versions supported by this tool. Exiting.");
		cleanup(1);
	}

	// Read the entire file into a buffer
	rewind(mainScmFile);	// IDK when the file pointer would have even been changed before this, but apparently this is needed
	mainScm = calloc(fileLength, 1);
	fread(mainScm, fileLength, 1, mainScmFile);
	//rewind(mainScmFile);

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////PROVIDING INFO AND TAKING INPUT FOR IP CHECK PARAMETERS/////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Print out all of the (actual) missions and their mission IDs
	for (uint8_t missionIndex = 2; missionIndex < 97; missionIndex++) {
		//offsetTemp += mission[missionIndex-1].size;
		printf("%d - %s\n", missionIndex, mission[missionIndex].name);
	}
	puts("");

	// Mission selection
	printf("Please enter the ID of the mission you would like to check for instapasses (2-96): ");
	scanf("%d", &missionSelect);
	printf("You selected %s (0x%X)\n", mission[missionSelect].name, mission[missionSelect].offset);
	if (missionSelect < AN_OLD_FRIEND || missionSelect > CHECKPOINT_CHARLIE) {
		puts("Invalid value. Exiting.");
		cleanup(1);
	}
	
	// Get IP check address range
	printf("Please enter the range of mission offsets you would like to check for instapasses for.\n");
	printf("Enter the mission offset range start (decimal): ");
	scanf("%u", &ipCheckOffsetStart);
	printf("Enter the mission offset range end (decimal): ");
	scanf("%u", &ipCheckOffsetEnd);

	if (ipCheckOffsetStart > ipCheckOffsetEnd) {
		puts("You entered an end smaller than the start of the range. Exiting.");
		cleanup(1);
	}

	// Prompt for checking the mission n missions prior to the mission-to-be-instapassed in the script
	printf("Enter number of missions back in the script you would like to test. For example, to test instapasses by starting the previous mission, enter 1. (0 if N/A) ");
	getchar();
	scanf("%hu", &numMissionsBack);
	if (numMissionsBack > missionSelect || numMissionsBack > 96) {
		puts("You entered an invalid value. Exiting.");
		cleanup(1);
	}
	
	if (numMissionsBack) {
		printf("Testing instapasses for %s when starting %s\n", mission[missionSelect].name, mission[missionSelect-numMissionsBack].name);
		// Increasing IP check offset range by the size of the additional missions
		for (char numMissionsBackCount = 1; numMissionsBackCount <= numMissionsBack; numMissionsBackCount++) {
			ipCheckOffsetStart += mission[missionSelect-numMissionsBackCount].size;
			ipCheckOffsetEnd += mission[missionSelect-numMissionsBackCount].size;

			jumpedMissionsSize += mission[missionSelect-numMissionsBackCount].size;
		}
	}

	printf("Checking mission offsets %d - %d\n", ipCheckOffsetStart, ipCheckOffsetEnd);

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////MAIN LOOP//////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Start the cursor where 

	// Loop through all (actual) missions
	for (int m = AN_OLD_FRIEND; m <= CHECKPOINT_CHARLIE; m++) {
		//curMisGlobalOffset += mission[m-1].size; // Get the global offset of the current mission.
		
		matchFound = 0;	// Reset flag indicating that at least one match has been found for this mission already
		if (mission[m].size < ipCheckOffsetStart)
			continue;	// Skip missions that are too short to possibly work
		if (m == missionSelect)
			continue;	// Skip checking the mission-to-be-instapassed because that would just be duping 

		mainScmCursor = mainScm + mission[m].offset + ipCheckOffsetStart; // The start of the area to check on this iteration

		// Main loop within the main loop. Check each byte within the provided range for anything matching a wait instruction
		for (int curLocalOffset = ipCheckOffsetStart; curLocalOffset <= ipCheckOffsetEnd; curLocalOffset++) {
			//printf("mainScm: %X. Mission offset: %X. IP check offset start: %u\n", (uint64_t)mainScm, mission[m].offset, ipCheckOffsetStart);
			if ((waitType = isWait())) {
				if (!matchFound) {	// If no match had been previously found for this mission already
					printf("\n%s", mission[m].name);	// Prints the mission name only on the first match of the mission
					matchFound = 1;
				}

				// Print results
				printf("\n\tWait found at mission offset %d ", curLocalOffset);
				printf("-> %d in %s", curLocalOffset + waitType - ((numMissionsBack)?jumpedMissionsSize:0), mission[missionSelect].name);

			}
			mainScmCursor++;
		} // Done checking current mission for waits
	} // All applicable missions checked
	printf("\n\nDone. Press Enter to exit.");
	getchar();
	getchar();
    cleanup(0);	// End prog successfully
}

void cleanup(int retNum) {
	if (mainScmFile)
		fclose(mainScmFile);
	
	free(mainScm);
	exit(retNum);
}

int isWait() {
	uint32_t currentInstruction;
	memcpy(&currentInstruction, mainScmCursor, sizeof(currentInstruction));
	//printf("Wait check bytes: %X\n", currentInstruction);
	// Check if matches either pattern 01 00 04 00 or 01 00 05 XX XX
	// The first case is pretty simple
	if (currentInstruction == 0x40001)
		return 4;

	currentInstruction &= 0x00FFFFFF;		// Otherwise we only want to check the three lower bytes, so clear out the MSByte.
	if (currentInstruction == 0x50001)
		return 5;
	else
		return 0;
}
/*// Printing out data (for testing)
		if (!(offset % 16))
			printf("Offset 0x%06x -- ", offset);
		printf("%02x ", byteVal);
		Sleep(5);
		if (!((offset+1) % 16))
			printf("\n");
		*/