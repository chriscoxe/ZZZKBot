# ZZZKBot - Changes

## Version 1.9.1

Version updated on GitHub (not for a particular competition). Notable differences:
* Feature: if exploring when selecting initial strategy, use sampling logic based on Thompson sampling (combined with some additional heuristics). Before, when sampling, it sampled from an even distribution across all available strategy combinations (combined with different heuristics) regardless of how successful that strategy combination has been in previous games, but was more greedy when it found a successful strategy combination. The new sampling logic is designed using a model that assumes that opponents do not adapt by learning from previous games, so it is intended to learn quicker in the long-term against opponents that do not adapt by learning from previous games, but is expected to perhaps perform worse in the long-term against opponents that adapt by learning from previous games. It's unclear how well it learns in the short-term compared to the previous version - it probably varies a lot depending on the opponent.
* Feature: inserted a config file setting (between the race field and the is4PoolBO field) for each configured strategy settings combination that controls whether it is just a hint (1) or not (0). If it is a hint then other strategy settings combinations may be used (e.g. explore other strategies after losing games, or if other strategies have won in any existing data) but if it is not a hint then the specified strategy settings combination will be used for all games in future (regardless of whether others have won in past games).
* Feature: add hard-coded creep location for more maps (i.e. just Roadkill, Polaris Rhapsody map version 1.0, Longinus 2).
* Change: remove tailored initial strategy parameters against all opponents.
* Change: changed the VCXPROJ file to upgrade from using Microsoft Visual Studio Community 2017 version 15.7.4 to using Microsoft Visual Studio Community 2017 version 15.9.64 with side-by-side minor version MSVC toolsets using the instructions at https://devblogs.microsoft.com/cppblog/side-by-side-minor-version-msvc-toolsets-in-visual-studio-2017/ to use the "VC++ 2017 version 15.7 v14.14 toolset" (which corresponds to Microsoft.VisualStudio.Component.VC.Tools.14.14, and use it to link with the re-release of BWAPI 4.2.0 (i.e. BWAPI.VS.15.7.3.7z as-is, without rebuilding it) (note: I previously used 15.7.4 and that works too).
* Fix: fixed a production freeze in the case where doing 4pool with numSunkens == 0 after a certain number of lings have died - the problem was that production freezes (no buildings/lings/drones/etc when I intended for it to keep making lings) until after the frame threshold is reached.
* Fix: fixed a bug where configuring race-specific number of sunkens always used 1 after their race is scouted (only a problem if you use the config file and it's a Random race opponent).
* Fix: fixed some compiler warnings, e.g. unused parameters/variables, deprecated functions, memory safety, casting issues, unecessary lambda/this captures. Seems to fix crashing on BASIL.
* Refactor: don't use static variables - make them class member variables.
* Refactor: use the override keyword where appropriate.

Note: this version is identical to the executable version that has been on SSCAIT since 2024-07-26 09:00:29 except that the version on SSCAIT has tailored initial strategy parameters against opponents (and the program version number is different). Also, I'm now adding some TXT files to this repository about some competitions that ZZZKBot competed in.

## Version 1.8.0

Version submitted on 10th July 2018 for the CIG 2018 Starcraft AI competition. The only noteworthy changes in this version are three small bugfixes to the learning algorithm (e.g. it was mistakenly using the is4PoolBO field as the timerAtGameStart field and this was causing it to mainly only alternate between two strategy setting combos; a bug with StratSettings comparator (i.e. comparator must satisfy strict weak ordering) which might have caused the learning logic to not work properly and was causing error pop-ups when run in Debug mode), learn plasma map independently to the other maps, re-added/added tailored initial strategy parameters against some opponents, blanked-out the DAT fields relating to system info such as CPU info and time zone, and version data files separately to the program version (set the data version to 1.7.0, so that data produced by version program 1.7.0 can be read). None of the underlying strategy logic has changed at all.

## Version 1.7.0

Version uploaded to SSCAIT bot name "Chris Coxe" on 7th November 2017, solely because some people wanted a more up-to-date version of ZZZKBot on SSCAIT that works on any map and works when playing as any player name. I am not intending to support/maintain/develop ZZZKBot in future and am not intending to compete in SCAIT 2017, although I haven't ruled either of them out either. This is just a release to allow people to more easily train their bots against ZZZKBot on their own environments and on SSCAIT/CIG/other maps. It is not as strong as the AIIDE 2017 or CIG 2017 versions of ZZZKBot because the hardcoded default strategy parameter values for opponents were removed in v1.6.0. If it is used in SSCAIT 2017 or other future competitions, I doubt it will do very well because of that, and also because so few games are played in SSCAIT compared with AIIDE/CIG, so it doesn't have much opportunity to learn before or during SSCAIT 2017.

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
