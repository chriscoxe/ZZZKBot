# ZZZKBot - ReadMe

## Description

ZZZKBot is a bot (AI) for Starcraft: Broodwar. It is designed to compete against other bots. It is not designed to compete against humans. It uses BWAPI as an API for interacting with Starcraft: Broodwar. See [AIIDE2017_ENTRY.txt](AIIDE2017_ENTRY.txt) for more information about the (1.5.0) version that was submitted for the 2017 AIIDE Starcraft AI competition.

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

## Why do I write Starcraft bots/AIs?

Writing a Starcraft bot is a good fit for my interests in problem solving, programming, software engineering, game theory, AI and machine learning, strategy games (Starcraft in particular), and competition. It's a lot of fun, and bot-versus-bot games are often farcical.

## Starcraft AIs Progress/Status

For my competition survey answers about developing Starcraft AIs, see the text files such as [competition_survey_AIIDE_2017_ZZZKBot.txt](competition_survey_AIIDE_2017_ZZZKBot.txt).  Here are some general thoughts on the status and future progress of Starcraft AIs:

The state of Starcraft AI at the time of writing (7th November 2017) is that it is still very narrow AI, even just within the game of Starcraft. By that, I mean that Starcraft bots currently still use a non-trivial amount of hard-coded logic and list/definition of features designed by humans just to enable a machine learning (ML) algorithm to be applied effectively (as opposed to the deep learning algorithm used by DeepMind's AlphaGo Zero program), and no one ML algorithm is being used yet to perform every kind of task that a player needs to perform in Starcraft in order to be able to be able to play the full game and be successful competitively.  By "tasks", I mean tasks like micromanaging individual or small groups of combat units, making more combat units and workers and buildings, deciding what the composition of your army should be, deciding what buildings to construct and where to place them, deciding what path each unit should move in, scouting to see what the enemy is doing and making use of that information, making more bases, harassing the enemy's bases, managing large battles, deciding where to attack and defend, special tactics, and decision making relating to long term strategy.

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