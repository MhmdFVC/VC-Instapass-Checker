# VC Instapass Checker
 
Checks VC mission scripts for potential instapasses. Supports both original and Haitian-friendly SCMs.
1) Enter the filename of the scm you wish to check. (If the filename is just main.scm, that is automatically loaded)
2) Enter a mission ID. You're given a list of all the missions with their IDs when you first open the tool.
3) Enter a range of mission-local offsets to check. Use the included disassembled scms with line offsets to help you out.
4) Optional: Specify a number of missions prior to your selected mission in the script (see mission ID list) to search for instapasses. For example, if you're looking for Taxi Driver instapass, you could enter 0 to just look for potential instapasses when just starting taxi itself, or specify 1 for Taxi Driver instapasses where you'd actually start Cabmaggedon.

At this point, all the missions will be searched for waits with offsets corresponding to your offset range. If there is a match, the mission and where the wait was found is printed to the console. **Since the tools to provide per-line global and local offsets use decimal numbers, everything entered and displayed is in decimal.**

## Tips on getting your offsets
You may have your own process for getting these, but I'll put this together as a quick overview of how I've been doing this since I was clueless about this for a long time.

You can be as liberal as you'd like in specifying the range, but to reduce false positives you should base the range loosely on the logic surrounding the 030C (progress made) or 0318 (save file name changed) opcode, or other relevant opcode where it may be plausible to achieve an instapass. The included disassembled scms with line offsets will help you here.

If an "instapass" won't land cleanly onto a new instruction, try taking a look at what "opcode" it will land on by opening the scm is a hex editor and navigating to the nearest global offset (warning: everything is little endian). Maybe by some miracle it will be something comprehensible that won't crash the game. The [Sanny Builder Library](https://library.sannybuilder.com/#/vc/script) is a good opcode reference, although it won't tell you the exact size of each instruction.

Hex editor example: opcode 03A4 will appear as A4 03, and a wait 1000 will appear as 01 00 05 E8 03 (01 00 - 0001, wait; 05 - 16-bit value?; E8 03 - 3E8, which is 1000 in hex). 
