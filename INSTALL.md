# ZZZKBot - Install

## Instructions to build ZZZKBot from source code

These instructions are simply the same as how you would compile BWAPI 4.2.0's ExampleAIModule project from source. Note that later versions of Windows should probably work too but I have not tested them.

1. On Windows (I used Windows 7 SP1), install Microsoft Visual Studio 2017 (I used Microsoft Visual Studio Community 2017; note that this is registerware, i.e. it can be downloaded for free from Microsoft without registering, but after 30 days it will prompt you and require you to register (free of charge) to be able to continue using it).

1. Install BWAPI 4.2.0 to a path that does not contain spaces. I am not sure whether the following instructions will work if you install it to a path that contains spaces - I haven't tried it.

1. Set the environment variable BWAPI_DIR to the path of where you installed BWAPI 4.2.0 to. Note: the project does not depend on any other libraries, does not reference any absolute file paths, and when the bot is run it does not require any absolute file paths (e.g. it doesn't matter what directory you have installed Starcraft: Broodwar to).

1. In Windows Explorer, double-click the ZZZKBot.vcxproj file.

1. Change the drop-down from "Debug" to "Release".

1. From the menu, select: Build -> Build Solution. It should build with no errors/warnings. On my computer it takes less than 15 seconds to build.

1. A folder named "Release" should be automatically created when building. The output is a single DLL file in that folder named ZZZKBot.dll.

## Instructions to install and run ZZZKBot

Note: ZZZKBot is intended to support being run on Windows 7 SP1 (later versions of Windows such as 8.1, 10 are hoped to work but have not been tested, and Vista has not been tested either). It has successfully been compiled on Windows 7 SP1 and the resulting DLL worked fine when run on Windows 7 SP1.

1. Install BWAPI 4.2.0 if you haven't already (and its dependencies, e.g. Broodwar).

1. The only file required is the DLL named ZZZKBot.dll from the "Release" folder. ZZZKBot does not need any other files to run, but note that it uses files in the "bwapi-data/read" and "bwapi-data/write" folders. Do not rename the DLL or the player name (should be "ZZZKBot"). Do not change the case-sensitivity of these letters.

1. If the machine where ZZZKBot will be run does not have Microsoft Visual Studio 2017 installed, you will need to install the 32-bit variant of the "Microsoft Visual C++ Redistributable for Visual Studio 2017" if not already installed (VC_redist_x86.exe (i.e. 32-bit) is only one you need for Windows 7 SP1). Currently it can be downloaded from Microsoft's website at [https://www.visualstudio.com/downloads/](https://www.visualstudio.com/downloads/) - be sure to install the x86 variant (the x64 variant is irrelevant).

1. Install and run the DLL with BWAPI version 4.2.0 as you would normally do for any other BWAPI AIModule bot that uses the "bwapi-data/read" and "bwapi-data/write" folders, e.g. copy the DLL to the following folder (create parent folders if necessary):

   \<Starcraft folder\>/bwapi-data/AI/

   and ensure that the following folders exist:

   \<Starcraft folder\>/bwapi-data/read/
   
   \<Starcraft folder\>/bwapi-data/write/

   and configure its path via the "ai" setting in:

   \<Starcraft folder\>/bwapi-data/bwapi.ini

   then run a launcher such as ChaosLauncher as Administrator and inject as usual, etc.

Notes: currently, ZZZKBot only properly supports playing as the Zerg race. ZZZKBot does not require any other DLLs apart from BWAPI 4.2.0. It does not use any 3rd-party libraries such as BWTA2/BWTA/BWEM at all. ZZZKBot uses disk I/O (i.e. has logic to use the "bwapi-data/read" and "bwapi-data/write" folders). It tailors its behavior to particular opponents based on in-game player names of the opponent bot(s).