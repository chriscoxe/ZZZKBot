# ZZZKBot - ReadMe

## Description

ZZZKBot is a bot (AI) for Starcraft: Broodwar. It is designed to compete against other bots. It is not designed to compete against humans. It uses BWAPI as an API for interacting with Starcraft: Broodwar. See [CIG2018_ENTRY.txt](CIG2018_ENTRY.txt) for more information about the (1.8.0) version that was submitted for the CIG 2018 Starcraft AI competition.

## Project Status

I am not intending to support/maintain/develop ZZZKBot in future, although I haven't ruled it out either. See [BUGS.md](BUGS.md).

In the longer term, I am more likely to start a new project than simply keep modifying the ZZZKBot project. I.E. with a different primary goal than just a Zerg bot that beats other bots in competitions.  E.g. beating humans, or playing as other race(s), or a bot that can play Broodwar & SC2, or a bot with much less hand-crafted logic, or write a bot properly from the ground up.  The approach I used to write ZZZKBot was to code it in the quickest possible time, at the expense of quality / attention to detail.

Support/donations/sponsorship or perhaps even job offers (would be my dream job!) to write or help write a "proper" Broodwar or SC2 bot that actually uses some sophisticated AI/ML techniques (perhaps alongside minimal hard-coded logic?), possibly eventually using more appropriate hardware would be helpful, i.e. via [https://www.paypal.me/quatari](https://www.paypal.me/quatari) or alternatively add me in LinkedIn but be sure to mention it's about Starcraft AI otherwise I may ignore connection requests.

So far, I've been working on my Starcraft bot sporadically just before competition deadlines just as a hobby, using simple techniques to reach low-hanging fruit against other bots, using my existing personal hardware, but I have been following the Broodwar and SC2 AI scene and papers closely and have a lot of ideas for more sophisticated (and resource-intensive...) AI/ML techniques I would like to experiment with in a bot and would love to work on something more serious/useful/ambitious in Broodwar/SC2 AI.

## Some things to know about ZZZKBot

* It's open-source, and 100% free - it uses the widely used "GNU Lesser General Public License (LGPL) version 3" license.
* It depends on other libraries, but they and their dependencies (and their dependencies, ad infinitum) are also open source and 100% free.
* The other libraries it depends on are as follows (their dependencies are not listed):
  * [BWAPI](https://github.com/bwapi/bwapi/)
    * BWAPI is an API for interacting with Starcraft: Broodwar. Starcraft and Starcraft: Broodwar are trademarks of Blizzard Entertainment. BWAPI is a third party "hack" that violates the End User License Agreement (EULA). BWAPI uses the "GNU Lesser General Public License (LGPL) version 3" license.

## License

See [LICENSE.txt](LICENSE.txt).

## Authors

See [AUTHORS.md](AUTHORS.md).

## Reporting Bugs

See [BUGS.md](BUGS.md).

## Release Notes / Version List / Change Log

See [CHANGES.md](CHANGES.md).

## Installation Instructions

See [INSTALL.md](INSTALL.md).

## Thanks

See [THANKS.md](THANKS.md).

## Project Resources

* Website: [https://chriscoxe.github.io/ZZZKBot/](https://chriscoxe.github.io/ZZZKBot/)
* Repository: [https://github.com/chriscoxe/ZZZKBot](https://github.com/chriscoxe/ZZZKBot)
* Releases: [https://github.com/chriscoxe/ZZZKBot/releases](https://github.com/chriscoxe/ZZZKBot/releases)
* Liquipedia page: [http://wiki.teamliquid.net/starcraft/ZZZKBot](http://wiki.teamliquid.net/starcraft/ZZZKBot)
* StarcraftAI.com page: [http://www.starcraftai.com/wiki/ZZZKBot](http://www.starcraftai.com/wiki/ZZZKBot)

## Competition Results

### ZZZKBot Competition Results

| Date | Rank | Prize Pool | Winnings | Result | Event | Type | Category |
| --- | --- | --- | --- | --- | --- | --- | -- |
| 2017-10-31 | N/A | | | Lost 0:1 vs [Stork](http://wiki.teamliquid.net/starcraft/Stork), <br> won 1:0 vs two amateurs | Sejong University showmatches ([press](https://www.technologyreview.com/s/609242/humans-are-still-better-than-ai-at-starcraftfor-now/), [details](https://cilab.sejong.ac.kr/home/doku.php?id=public:starcraft_human_vs_ai)) | Man-vs-Machine | Full game |
| 2017-10-09 | 1/28 | | | 83.11% | [AIIDE2017](http://wiki.teamliquid.net/starcraft/AIIDE) ([details](https://www.cs.mun.ca/~dchurchill/starcraftaicomp/2017/), [press](https://www.wired.com/story/facebook-quietly-enters-starcraft-war-for-ai-bots-and-loses/)) | Bot-vs-Bot | Full game |
| 2017-08-24 | 1/20 | USD 1000 | USD 500 but ineligible (not a student or young professional) | 82.06% | [CIG2017](http://wiki.teamliquid.net/starcraft/CIG) ([details](https://cilab.sejong.ac.kr/sc_competition/?cat=17)) | Bot-vs-Bot | Full game |
| 2016-12-18 | 4/45 in round robin phase, <br> quarterfinalist in elimination phase | | | 88% (i.e. 4th) in round robin phase, <br> elimination phase: lost 0:2 vs [Iron](http://bwem.sourceforge.net/Iron.html) in Quarterfinals | [SSCAIT2016](http://wiki.teamliquid.net/starcraft/SSCAIT2016) ([details](http://www.sscaitournament.com/index.php?action=2016)) | Bot-vs-Bot | Mixed division (i.e. non students) |
| 2016-10-14 | 2/21 | | | 85.05% | [AIIDE2016](http://wiki.teamliquid.net/starcraft/AIIDE) ([details](https://www.cs.mun.ca/~dchurchill/starcraftaicomp/2016/)) | Bot-vs-Bot | Full game |
| 2016-09-23 | 4/16 | USD 1000 | USD 0 | 53.08% (4th) in final phase, <br> 69.18% (7th) in qualifier phase. <br> Note: weak because outdated (submitted old AIIDE 2015 version) | [CIG2016](http://wiki.teamliquid.net/starcraft/CIG) ([details](https://sites.google.com/site/starcraftaic/result)) | Bot-vs-Bot | Full game |
| 2016-01-30 | 4/42 | | | 84.44% (i.e. 4th), <br> Elimination phase: lost 2:1 vs [KillerBot](http://wiki.teamliquid.net/starcraft/Killerbot) in Semifinals, <br> lost 0:2 vs [Stone](http://wiki.teamliquid.net/starcraft/Stone) in Bronze match | [SSCAIT2015](http://wiki.teamliquid.net/starcraft/SSCAIT2015) ([details](http://www.sscaitournament.com/index.php?action=2015), [history](http://www.cs.mun.ca/~dchurchill/starcraftaicomp/history.shtml)) | Bot-vs-Bot | Mixed division (i.e. non students) |
| 2015-11-17 | 2/22 (or 1/22?) | | | 87.83% (officially announced as a statistical tie for 1st place), <br> lost 0:2 vs [Djem5](http://wiki.teamliquid.net/starcraft/Djem5) in [man-vs-machine games](http://www.cs.mun.ca/~dchurchill/starcraftaicomp/report2015.shtml#mvm) | [AIIDE2015](http://wiki.teamliquid.net/starcraft/AIIDE2015) ([details](https://www.cs.mun.ca/~dchurchill/starcraftaicomp/2015/), [report](http://www.cs.mun.ca/~dchurchill/starcraftaicomp/report2015.shtml), [history](http://www.cs.mun.ca/~dchurchill/starcraftaicomp/history.shtml)) | Bot-vs-Bot (plus misc man-vs-machine games) | Full game |

### ZZZBot Competition Results

| Date | Rank | Prize Pool | Winnings | Result | Event | Type | Category |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 2015-09-02 | 1/14 | USD 1000 | USD 500 | 81.03% | [CIG2015](http://wiki.teamliquid.net/starcraft/CIG2015) ([details](https://cilab.sejong.ac.kr/sc_competition2015/#Results), [history](http://www.cs.mun.ca/~dchurchill/starcraftaicomp/history.shtml)) | Bot-vs-Bot | Full game |

## Why do I write Starcraft bots/AIs?

Writing a Starcraft bot is a good fit for my interests in problem solving, programming, software engineering, game theory, AI and machine learning, strategy games (Starcraft in particular), and competition. It's a lot of fun, and bot-versus-bot games are often farcical.

## Starcraft AIs Progress/Status

For my competition survey answers about developing Starcraft AIs, see the text files such as [competition_survey_CIG_2018_ZZZKBot.txt](competition_survey_CIG_2018_ZZZKBot.txt).  Here are some general thoughts on the status and future progress of Starcraft AIs:

The state of Starcraft AI at the time of writing (10th July 2018) is that it is still very narrow AI, even just within the game of Starcraft. By that, I mean that Starcraft bots currently still use a non-trivial amount of hard-coded logic and list/definition of features designed by humans just to enable a machine learning (ML) algorithm to be applied effectively (as opposed to the deep learning algorithm used by DeepMind's AlphaGo Zero program), and no one ML algorithm is being used yet to perform every kind of task that a player needs to perform in Starcraft in order to be able to be able to play the full game and be successful competitively.  By "tasks", I mean tasks like micromanaging individual or small groups of combat units, making more combat units and workers and buildings, deciding what the composition of your army should be, deciding what buildings to construct and where to place them, deciding what path each unit should move in, scouting to see what the enemy is doing and making use of that information, making more bases, harassing the enemy's bases, managing large battles, deciding where to attack and defend, special tactics, and decision making relating to long term strategy.

### Progress through Starcraft AI Modularity

On a general note, I think it is a pity that so many bots and libraries are developed with the author's intention for it to be used by other botters as a preferred library/framework to use for basic functionality, only to end up hardly being used because others perceive it as being too difficult to call or incorporate or copy-and-paste in their bot (due to the lack of inherent modularity in BWAPI bots, and compatibility problems). Some notable exceptions are terrain libraries ([BWTA](https://code.google.com/p/bwta/) then [BWTA2](https://bitbucket.org/auriarte/bwta2) and [BWEM](http://bwem.sourceforge.net/) are widely used) and Java wrappers ([JNIBWAPI](https://github.com/JNIBWAPI/JNIBWAPI) then [BWMIRROR](https://github.com/vjurenka/BWMirror) and [BWAPI4J](https://github.com/OpenBW/BWAPI4J)) and base bots (see an interesting family tree of some bots [here](http://www.teamliquid.net/blogs/518904-family-tree-of-starcraft-bots)). I think a few of the reasons so many people write basic functionality for other bots to use are because:

1. BWAPI does not force everyone to implement a interface whose runtime target (DLL or EXE or e.g. use a protobuf-like protocol for control) can definitely be run/wrapped by other bots as part of their bot out-of-the-box without ever needing to recompile it. E.g. so the authors of [MegaBot](https://github.com/andertavares/MegaBot) could trivially have implemented a wrapper that simply dynamically calls the pre-existing DLLs for Skynet/NUSBot/Xelnaga (and many other Protoss bots) and that is all they need to do. Or if BWAPI migrates to a later version of Visual Studio that uses a different toolset version, avoid the requirement to need to recompile all bot/library source code in order for it to be compatible. Imagine how fast progress would be if everyone could easily include the runtime targets of other bots as part of their own bot (with some way of avoiding interference between sub-bots, e.g. some bots write to the same file paths unfortunately). You could train a true "Mega"/"Meta" Bot that can run all bots (including all races, and all bots regardless of whether they would otherwise have been DLL or EXE or DLL client or Java bots) and learns how to pick sub-bot according to opponent race and/or map size and/or map hash and/or starting location (and/or opponent player name if the rules allow it). The interface could also enforce some logic to sandbox sub-bots (so they can't interfere with each other) and control each sub-bot's understanding of how it is being run within another (meta) bot, e.g. expose the results of games where this sub-bot was used, and expose the results of games where other sub-bots were used - similarly to how MegaBot tracks this info.

1. Unfortunately, if you implement BWAPI's client-server model (BWAPI::Client class) it doesn't just implement a sub-bot - it also prevents you from wrapping the sub-bot within a (meta) bot, because it implements the logic that connects to a game and starts a game. It would be great if everyone had the ability to just produce a sub-bot. If some people want to prevent their bot from being run within other bots, personally I don't mind them being able to do that, but I suggest that by default, it should not work that way. I.E. the default/recommended way of writing a bot should be to just implement a sub-bot (i.e. similar to a BWAPI::AIModule (like a DLL, not like an EXE/JAR) or a BWAPI::Client (like a DLL, not like an EXE/JAR), but abstracted via some kind of protobuf-like interface that means it can run on a remote different operating system (i.e. programming language agnostic and operating-system agnostic and doesn't matter whether a sub-bot requires Windows XP or 7 or 8 or 10 or Linux or Mac or whatever)).

1. BWAPI does not allow bots or sub-bots to use earlier versions of BWAPI. BWAPI version 3.X allowed this to a certain extent, but this feature was scrapped (I don't know why). I realize that backwards-compatibility may not be a good idea for competitions (because old bots may unintentionally "cheat" by making use of game state information they shouldn't have been allowed to see) but it would be handy. I also realize that it may not be easy to enable BWAPI to start a (meta) bot that uses BWAPI 4.2.0 but may call sub-bot(s) that require BWAPI 4.1.2 / 3.7.5 / 3.7.4 etc. I'm not sure how to solve this - it may require a more disruptive design change in BWAPI to solve it unfortunately.

I like the inherent modularity of libraries like [BWSAL](https://code.google.com/archive/p/bwsal/)/[BWSAL2](https://github.com/Fobbah/bwsal) and the approaches to modularity such as having individual managers (ProductionManager, GathererManager etc) that cooperate in a more modular fashion. Unfortunately, it wouldn't be useful unless BWAPI is changed to work more in the way I described. The more modular bots are, the easier it is to call or incorporate or copy-and-paste logic from other bots. One project I've been thinking of attempting for some time is a recommended protocol for bots/sub-bots and/or individual modules to interact, including a more sophisticated arbitration system than [BWSAL](https://code.google.com/archive/p/bwsal/)/[BWSAL2](https://github.com/Fobbah/bwsal) (or define various policies that enable users to control what kinds of control/arbitration approaches their bot supports).

## What additional challenges are there when making an AI for Starcraft compared with games like Go and chess?

Some differences are:
1. Starcraft is a game of incomplete information - the game state is only partially observable, mainly due to the "fog of war", i.e. a player's vision of the map is limited to the areas of the map within the sight ranges of their units.  This is analogous to a variant of chess called [Dark Chess](https://en.wikipedia.org/wiki/Dark_chess) where a player does not see the entire board, only their own pieces and the squares which they can legally move to.  Players need to scout the opponent in order to gather information about what they are doing.
1. It's not a turn-based game - it is played in real time, and decisions need to be made quickly in real time. The rate that a player can effectively perform actions (using a real keyboard/mouse as a human, or e.g. via an imaginary keyboard/mouse as a bot) may have a large impact on how they plan and perform their strategies and tactics. Also, players can issue actions simultaneously, so a player can't necessarily wait to see what their opponent does before deciding what they want to do like they would in Go or chess.
1. The game engines for Starcraft: Brood War and Starcraft 2 are closed source software. An accurate open-source emulator for the Starcraft: Brood War game engine has only just been implemented within the last year (by Vegard Mella, also known as handle "tscmoo", in the [OpenBW](http://www.openbw.com/) project). There is currently no open-source emulator for Starcraft 2.  Open-source emulators are useful for being able to run search algorithms, and the emulation speed affects the speed of search algorithms and the speed of running training algorithms for machine learning. Emulators for Go and chess are very fast, but emulators for Starcraft are significantly slower because the game is much more complicated.
1. Even if there had been no fog of war, the game engine would still not be completely predictable because it uses pseudo-random numbers for some calculations, e.g. combat units have a probability of missing their targets when attacking units at higher ground.  So, Starcraft is not completely deterministic unless the random seed is also taken into account in the game state - instead, it is stochastic.
1. For the above reasons, it is only possible to build approximations of game simulations, so any search algorithms used do not have complete access to the game state information, and need a way of dealing with this problem.
1. Each player has many units and each unit can be issued actions individually and simultaneously to their other units (i.e. multi-agent), and there is a small delay between when each action is issued and actually executed.  Also, games like Starcraft often require especially precisely coordinated timing of sequences of actions by multiple units.
1. Compared to games like Go and chess, Starcraft has a huge number of possible game states and large number of actions allowed at any moment, even if you ignore the fact that units can be issued actions simultaneously, i.e. the game tree of possible game states and actions has a huge branching factor compared with games like Go and chess.  Even just considering the size of the map (which is a small part of the game state), the maps are huge compared to the size of the grid used in Go or the 8x8 board in chess.
1. Compared to games like Go and chess, AI techniques that use parameters or "features" as inputs may potentially take many more possible dimensions of features as inputs, whereas the current state of the game can be described relatively concisely for Go and chess. For example, each unit in Starcraft has a large number of attributes such as hit points, shields, armor, weapon cool-down times, location on the map, direction it is facing, velocity.  Picking what features to query for in order to provide their values as inputs to AI techniques becomes a whole problem in itself, especially considering that actions must be issued in real-time and there is limited computation time allowed (in most competitions only about 55 milliseconds maximum is allowed per frame, with 24 frames per second).
1. In comparison to games like Go and chess, it is often hard to get a rough estimate of who is winning just by looking at a small amount of the game state such as what pieces are currently on the board and where they are on the board.  It's a similar issue to the last point.
1. A full bot-versus-bot game (for existing bots anyway) typically takes a long time to play out in real-world time (typically between 20 seconds and 10 minutes), even if some tricks are used to speed up how fast games are played on servers.  This slows down the pace of development/testing and may slow down the pace of how fast AIs can effectively be trained.
1. There is a large variety of maps, and many have distinctive properties that may have a large impact on players' long term strategy plans and play styles.
1. Currently, it is time-consuming and may be tricky to set up and run bot-versus-bot games properly in an automated fashion - some background knowledge of the platforms/tools involved is needed, e.g. how to set up virtual machines.