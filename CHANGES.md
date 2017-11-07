# ZZZKBot - Changes

## Version 1.7.0

Version uploaded to SSCAIT bot name "Chris Coxe" on 7th November 2017, solely because some people wanted a more up-to-date version of ZZZKBot on SSCAIT that works on any map and works when playing as any player name. I am not intending to support/maintain/develop ZZZKBot in future and am not intending to compete in SCAIT 2017, although I haven't ruled either of them out either. This is just a release to allow people to more easily train their bots against ZZZKBot on their own environments and on SSCAIT/CIG/other maps. It is not as strong as the AIIDE 2017 or CIG 2017 versions of ZZZKBot
because the hardcoded default strategy parameter values for opponents were removed in v1.6.0. If it is used in SSCAIT 2017 or other future competitions, I doubt it will do very well because of that, and also because so few games are played in SSCAIT compared with AIIDE/CIG, so it doesn't have much opportunity to learn before or during SSCAIT 2017.

The changes in this version are that it is now fixed so it works properly on any map and when playing as any player name (thanks Jaj22!), added hardcoded creep locations for SSCAIT & CIG maps, added special logic and fixes for Plasma map (was included in CIG 2017 version), added ability to use an optional config file to tailor strategy parameter values to opponents (the file shouldn't exist by default), fixes/improvements to the buggy learning logic that was causing it to learn unnecessarily slowly, fixed a problem relating to the learning logic where the bot was getting stuck waiting forever for a sunken to be made when particular combinations of strategy parameter values were being used, minor updates to TXT file documentation (including adding old competition survey answers), converted some TXT files to MD files, added comments for map names and versions etc (incl. AIIDE, SSCAIT, CIG maps) for clarity, added some comments to explain some logic, added some old commented-out code just FTR.

## Version 1.6.0

Version uploaded to SSCAIT bot name "Chris Coxe" on 1st November 2017, solely because some people wanted a more up-to-date version of ZZZKBot on SSCAIT.

The changes are that my player name was fixed for SSCAIT, added support for SSCAIT maps (thanks Jaj22!) (creep location info is still missing though), increased the file format version number from 1.5.0 to 1.6.0, removed all references to other AIIDE 2017 player names.

## Version 1.5.0

Version submitted on 1st September 2017 for the 2017 AIIDE Starcraft AI competition. Some major changes are that it now uses BWAPI 4.2.0 (not 4.1.2), requires the "Microsoft Visual C++ Redistributable for Visual Studio 2017" (x86 variant) to run (not 2013), requires Visual Studio 2017 to build (not 2013), now uses the "bwapi-data/read" and "bwapi-data/write" folders for learning purposes, and no longer runs on Windows XP SP3 (because BWAPI version 4.2.0 doesn't).

## Version 1.4.0

Version submitted on 16th July 2017 for the CIG 2017 Starcraft AI competition. It has logic to read and write files but only uses them for logging/record-keeping purposes - it does not use them for any learning logic.

## Version 1.3

Version submitted on 19th December 2016 for the SSCAIT 2016 Starcraft AI competition.

## Version 1.2

Version submitted on 7th September 2016 for the AIIDE 2016 Starcraft AI competition. See file "SCAI2016_ENTRY.txt" for submission details. The instructions to build and install are the same as AIIDE 2015 except that I inserted this step in INSTALL.txt that I forgot to write in AIIDE 2015:

```text
3. If the machine where ZZZKBot will be run does not have Microsoft Visual Studio 2013 installed, you will need to install the 32-bit variant of the "Visual C++ Redistributable Packages for Visual Studio 2013" if not already installed (vcredist_x86.exe (i.e. 32-bit) is only one you need for Windows XP). Currently it can be downloaded from Microsoft's website at: https://www.microsoft.com/en-au/download/details.aspx?id=40784
```

## Version 1.1

Version submitted on 24th December 2015 for the SSCAIT 2015 Starcraft AI competition.

## Version 1.0

Version submitted on 10th October 2015 for the AIIDE 2015 Starcraft AI competition. See file "SCAI2015_ENTRY.txt" for submission details. ZZZKBot is an improved and heavily refactored version of ZZZBot (ZZZBot won first rank in the CIG 2015 Starcraft AI competition).