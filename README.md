# VC Instapass Checker
 
Checks for potential instapasses. Only supports JP for now.
1) Enter a mission ID. You're given a list of all the missions with their IDs when you first open the tool.
2) Enter a range of global or local (to your selected mission) offsets in main.scm to check.
4) Specify a number of missions prior to your selected mission in the script (see mission ID list) to search for instapasses. For example, if you're looking for Taxi Driver instapass, you could enter 0 to just look for potential instapasses when just starting taxi itself, or specify 1 for Taxi Driver instapasses where you'd actually start Cabmaggedon.
5) Any mission longer than the final (local) offset range will then be searched for waits within the offset range. If there is a match, the mission and where the wait was found is printed to the console.

## Tips on getting your offsets
You may have your own process for getting these, but I'll put this together as a quick overview of how I've been doing this since I was clueless about this for a long time.

You can be as liberal as you'd like in specifying the range, but to reduce false positives you should base the range loosely on the logic surrounding the 030C (100%) or 0318 (All Missions) opcode, or other relevant opcode, where it may be plausible to achieve an instapass. I suggest looking for your approximate offset range in a Sanny Builder decompile of main.scm generated with opcodes and global+local offset labels (included) so that you can find the exact offsets you need more easily when viewing the SCM in a hex editor.

You can also use a hex editor to see where precisely an "instapass" will put you. Global offset labels may be helpful for jumping straight to a command. Note that when viewing in a hex editor, multi-byte values are stored in little endian order; e.g. opcode 03A4 will appear as A4 03, and a wait 1000 will appear as 01 00 05 E8 03 (01 00 - 0001, wait; 05 - short data type?; E8 03 - 3E8, which is 1000 in hex). 

## Disclaimer
This was primarily done as an exercise for myself, but I believe it could have some use. Any feedback on improvements is welcome.
