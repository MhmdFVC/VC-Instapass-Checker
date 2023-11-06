# VC Instapass Checker
 
Checks for instapasses. Only supports JP for now.
1) Enter a mission ID. You're given a list of all the missions with their IDs when you first open the tool.
2) Enter a range of global or local (to your selected mission) offsets in main.scm to check. You can be a bit liberal in specifying the range, but just look for logic before the 030C (100%) or 0318 (All Missions) opcode, or other relevant opcode, where it may be plausible to achieve an instapass.
4) Specify if you want to perform the search with some mission which is situated prior to your selected mission in the script (see mission ID list). For example, if you're looking for Taxi Driver instapass, you could enter 0 to just look for potential instapasses when starting taxi itself, or specify 1 for Taxi Driver instapasses where you'd actually start Cabmaggedon. I'm honestly not sure if this works exactly the way I think it does, but some implementation is there (it just adds the sizes of the preceding missions to your specified offset range).
5) Any mission longer than the final (local) offset range will then be searched for waits within the offset range. If there is a match, the mission and where the wait was found is printed to the console.

## Tips on getting your offsets
You may have your own process for getting these, but I'll put this together as a quick overview of how I've been doing this since I was clueless about this for a long time.

I suggest looking for your approximate offset range in a Sanny Builder decompile of main.scm generated with opcodes and hexadecimal labels (provided in the zip file/repo) so that you can find the exact offsets you need more easily when viewing the SCM in a hex editor. Global offset labels may be more helpful so you can jump straight to that area in a hex editor using go to offset. Note that multi-byte values are stored in big endian order; e.g. opcode 03A4 will appear as A4 03, and a wait 1000 will appear as 01 00 05 E8 03 (01 00 - 0001, wait; 05 - length of command?; E8 03 - 3E8, which is 1000 in hex). Also note that text labels are present as-is in the SCM, so you can do a text search for something like 'LAW2_3' to easily reach approximate parts of the script. 

The easiest way to find your exact range will be using global offsets as well - you won't need to find the exact start of your mission and can just see the range right away if you highlight your bytes using a good hex editor like HxD. See below. The range is presented in the bottom information bar after highlighting a block immediately preceding Taxi Driver's 030C opcode.

![image](https://github.com/MhmdFVC/VC-Instapass-Checker/assets/18182748/35b07035-e656-423d-810d-273b039fe09f)
