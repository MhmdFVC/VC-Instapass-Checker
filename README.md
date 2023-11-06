# VC Instapass Checker
 
Checks for potential instapasses. Only supports JP for now.
1) Enter a mission ID. You're given a list of all the missions with their IDs when you first open the tool.
2) Enter a range of global or local (to your selected mission) offsets in main.scm to check.
4) Specify a number of missions prior to your selected mission in the script (see mission ID list) to search for instapasses. For example, if you're looking for Taxi Driver instapass, you could enter 0 to just look for potential instapasses when just starting taxi itself, or specify 1 for Taxi Driver instapasses where you'd actually start Cabmaggedon. I'm honestly not sure if this works exactly the way I think it does, but some implementation is there (it just adds the size(s) of the preceding mission(s) to your specified offset range).
5) Any mission longer than the final (local) offset range will then be searched for waits within the offset range. If there is a match, the mission and where the wait was found is printed to the console.

## Tips on getting your offsets
You may have your own process for getting these, but I'll put this together as a quick overview of how I've been doing this since I was clueless about this for a long time.

You can be as liberal as you'd like in specifying the range, but to reduce false positives you should base the range loosely on the logic surrounding the 030C (100%) or 0318 (All Missions) opcode, or other relevant opcode, where it may be plausible to achieve an instapass. I suggest looking for your approximate offset range in a Sanny Builder decompile of main.scm generated with opcodes and hexadecimal labels (provided in the zip file/repo) so that you can find the exact offsets you need more easily when viewing the SCM in a hex editor. Global offset labels may be more helpful so you can jump straight to that area in a hex editor using go to offset. Note that multi-byte values are stored in little endian order; e.g. opcode 03A4 will appear as A4 03, and a wait 1000 will appear as 01 00 05 E8 03 (01 00 - 0001, wait; 05 - length of command?; E8 03 - 3E8, which is 1000 in hex). Also note that text labels are present as-is in the SCM, so you can do a text search for something like 'LAW2_3' to easily reach approximate parts of the script. 

The easiest way to find your exact range will be using global offsets as well - you won't need to find the exact start of your mission and can just see the range right away if you highlight your bytes using a good hex editor like HxD. See below. The range is presented in the bottom information bar after highlighting a block immediately preceding Taxi Driver's 030C opcode.

![image](https://github.com/MhmdFVC/VC-Instapass-Checker/assets/18182748/35b07035-e656-423d-810d-273b039fe09f)


## Disclaimer
If you know this stuff better than I do, feel free to tell me about it and how. This was primarily done as an exercise for myself, but I believe it could have some use since I don't think another tool like this is publicly available, and any feedback on improvements that should be made are welcome. I am also rather fuzzy about what makes missions require "stack fixing", but that could perhaps be another automated check that would be beneficial.
