// Copyright 2017 Chris Coxe.
// 
// ZZZKBot is distributed under the terms of the GNU Lesser General
// Public License (LGPL) version 3.
//
// This file is part of ZZZKBot.
// 
// ZZZKBot is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// ZZZKBot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with ZZZKBot.  If not, see <http://www.gnu.org/licenses/>.
// 
// This file was created by copying then modifying file
// ExampleAIModule/Source/ExampleAIModule.h
// of BWAPI version 4.1.2
// (https://github.com/bwapi/bwapi/releases/tag/v4.1.2 which is
// distributed under the terms of the GNU Lesser General Public License
// (LGPL) version 3), then updating it to be compatible with a later version
// of BWAPI.

#pragma once
#include <BWAPI.h>
#include <set>
#include <ctime>

// Reminder: don't use "Broodwar" in any global class constructor!

class ZZZKBotAIModule : public BWAPI::AIModule
{
public:
    // Virtual functions for callbacks, leave these as they are.
    virtual void onStart();
    virtual void onEnd(bool isWinner);
    virtual void onFrame();
    virtual void onSendText(std::string text);
    virtual void onReceiveText(BWAPI::Player player, std::string text);
    virtual void onPlayerLeft(BWAPI::Player player);
    virtual void onNukeDetect(BWAPI::Position target);
    virtual void onUnitDiscover(BWAPI::Unit unit);
    virtual void onUnitEvade(BWAPI::Unit unit);
    virtual void onUnitShow(BWAPI::Unit unit);
    virtual void onUnitHide(BWAPI::Unit unit);
    virtual void onUnitCreate(BWAPI::Unit unit);
    virtual void onUnitDestroy(BWAPI::Unit unit);
    virtual void onUnitMorph(BWAPI::Unit unit);
    virtual void onUnitRenegade(BWAPI::Unit unit);
    virtual void onSaveGame(std::string gameName);
    virtual void onUnitComplete(BWAPI::Unit unit);
    // Everything below this line is safe to modify.

    const std::string startOfLineSentinel = "||->";
    const std::string endOfLineSentinel = "<-||";
    const std::string startOfUpdateSentinel = "|->";
    const std::string endOfUpdateSentinel = "<-|";

    const std::string delim = "\t";

    const std::string initUpdateSignifier = "init";
    const std::string raceScoutedUpdateSignifier = "raceScouted";
    const std::string onPlayerLeftUpdateSignifier = "onPlayerLeft";
    const std::string onEndUpdateSignifier = "onEnd";

    int enemyPlayerID = -1;
    std::string enemyWriteFilePath;

    std::time_t timerAtGameStart = std::time(nullptr);

    struct StratSettings
    {
        bool is4PoolBO;
        bool isSpeedlingBO;
        bool isHydraRushBO;
        bool isMutaRushBODecidedAfterScoutEnemyRace;
        bool isMutaRushBO;

        // For normal queries, query isMutaRushBO, not these because they are only used
        // in conjunction with isMutaRushBODecidedAfterScoutEnemyRace.
        bool isMutaRushBOVsProtoss;
        bool isMutaRushBOVsTerran;
        bool isMutaRushBOVsZerg;

        bool isSpeedlingPushDeferred;
        bool isEnemyWorkerRusher;
        bool isNumSunkensDecidedAfterScoutEnemyRace;
        int numSunkens;

        // For normal queries, query numSunkens, not these because they are only used
        // in conjunction with isNumSunkensDecidedAfterScoutEnemyRace.
        int numSunkensVsProtoss;
        int numSunkensVsTerran;
        int numSunkensVsZerg;

        inline bool operator <(const StratSettings& other) const
        {
            if (!is4PoolBO && other.is4PoolBO) return true;
            if (is4PoolBO && !other.is4PoolBO) return false;
            if (!isSpeedlingBO && other.isSpeedlingBO) return true;
            if (isSpeedlingBO && !other.isSpeedlingBO) return false;
            if (!isHydraRushBO && other.isHydraRushBO) return true;
            if (isHydraRushBO && !other.isHydraRushBO) return false;
            if (!isMutaRushBODecidedAfterScoutEnemyRace && other.isMutaRushBODecidedAfterScoutEnemyRace) return true;
            if (isMutaRushBODecidedAfterScoutEnemyRace && !other.isMutaRushBODecidedAfterScoutEnemyRace) return false;
            if (!isMutaRushBO && other.isMutaRushBO) return true;
            if (isMutaRushBO && !other.isMutaRushBO) return false;
            if (!isMutaRushBOVsProtoss && other.isMutaRushBOVsProtoss) return true;
            if (isMutaRushBOVsProtoss && !other.isMutaRushBOVsProtoss) return false;
            if (!isMutaRushBOVsTerran && other.isMutaRushBOVsTerran) return true;
            if (isMutaRushBOVsTerran && !other.isMutaRushBOVsTerran) return false;
            if (!isMutaRushBOVsZerg && other.isMutaRushBOVsZerg) return true;
            if (isMutaRushBOVsZerg && !other.isMutaRushBOVsZerg) return false;
            if (!isSpeedlingPushDeferred && other.isSpeedlingPushDeferred) return true;
            if (isSpeedlingPushDeferred && !other.isSpeedlingPushDeferred) return false;
            if (!isEnemyWorkerRusher && other.isEnemyWorkerRusher) return true;
            if (isEnemyWorkerRusher && !other.isEnemyWorkerRusher) return false;
            if (!isNumSunkensDecidedAfterScoutEnemyRace && other.isNumSunkensDecidedAfterScoutEnemyRace) return true;
            if (isNumSunkensDecidedAfterScoutEnemyRace && !other.isNumSunkensDecidedAfterScoutEnemyRace) return false;
            if (numSunkens < other.numSunkens) return true;
            if (numSunkens > other.numSunkens) return false;
            if (numSunkensVsProtoss < other.numSunkensVsProtoss) return true;
            if (numSunkensVsProtoss > other.numSunkensVsProtoss) return false;
            if (numSunkensVsTerran < other.numSunkensVsTerran) return true;
            if (numSunkensVsTerran > other.numSunkensVsTerran) return false;
            if (numSunkensVsZerg < other.numSunkensVsZerg) return true;
            if (numSunkensVsZerg > other.numSunkensVsZerg) return false;
            return true;
        }

        inline bool operator ==(const StratSettings& other) const
        {
            if (is4PoolBO != other.is4PoolBO) return false;
            if (isSpeedlingBO != other.isSpeedlingBO) return false;
            if (isHydraRushBO != other.isHydraRushBO) return false;
            if (isMutaRushBODecidedAfterScoutEnemyRace != other.isMutaRushBODecidedAfterScoutEnemyRace) return false;
            if (isMutaRushBO != other.isMutaRushBO) return false;
            if (isMutaRushBOVsProtoss != other.isMutaRushBOVsProtoss) return false;
            if (isMutaRushBOVsTerran != other.isMutaRushBOVsTerran) return false;
            if (isMutaRushBOVsZerg != other.isMutaRushBOVsZerg) return false;
            if (isSpeedlingPushDeferred != other.isSpeedlingPushDeferred) return false;
            if (isEnemyWorkerRusher != other.isEnemyWorkerRusher) return false;
            if (isNumSunkensDecidedAfterScoutEnemyRace != other.isNumSunkensDecidedAfterScoutEnemyRace) return false;
            if (numSunkens != other.numSunkens) return false;
            if (numSunkensVsProtoss != other.numSunkensVsProtoss) return false;
            if (numSunkensVsTerran != other.numSunkensVsTerran) return false;
            if (numSunkensVsZerg != other.numSunkensVsZerg) return false;
            return true;
        }
    } ss = {};

    // The key is the game ID.
    std::map<int, StratSettings> gameIDToStratSettings;

    // The key is the game ID. The value is the frame count in onEnd().
    std::map<int, int> gameIDToOnEndFrameCount;

    struct GameIDSet
    {
        // The value is the game ID.
        std::set<int> val;
    };

    struct TimerAtGameStartMap
    {
        // The key is the timer at the start of the game.
        std::map<int, GameIDSet> val;
    };

    struct OutcomeMap
    {
        // The key is whether the value is about wins (i.e. true) or losses (i.e. false).
        std::map<bool, TimerAtGameStartMap> val;

        // The key is whether the value is about wins (i.e. true) or losses (i.e. false).
        std::map<bool, std::map<StratSettings, int> > numOutcomes;

        int gameIDIfWonLastGame = -1;
        int gameIDIfLostLastGame = -1;
    };

    struct EnemyStartLocDeducedMap
    {
        // The key is the enemy start location if it is known at the start of the
        // game (e.g. if the CompleteMapInformation flag is enabled) or if it is.
        // deduced at the start of the game, otherwise it is Unknown.
        std::map<const BWAPI::TilePosition, OutcomeMap> val;

        // The key is whether the value is about wins (i.e. true) or losses (i.e. false).
        std::map<bool, std::map<StratSettings, int> > numOutcomes;

        int gameIDIfWonLastGame = -1;
        int gameIDIfLostLastGame = -1;
    };

    struct MyStartLocationMap
    {
        // The key is my start location.
        std::map<const BWAPI::TilePosition, EnemyStartLocDeducedMap> val;

        // The key is whether the value is about wins (i.e. true) or losses (i.e. false).
        std::map<bool, std::map<StratSettings, int> > numOutcomes;

        int gameIDIfWonLastGame = -1;
        int gameIDIfLostLastGame = -1;
    };

    struct MapHashMap
    {
        // The key is the mapHash().
        std::map<std::string, MyStartLocationMap> val;

        // The key is whether the value is about wins (i.e. true) or losses (i.e. false).
        std::map<bool, std::map<StratSettings, int> > numOutcomes;

        int gameIDIfWonLastGame = -1;
        int gameIDIfLostLastGame = -1;
    };

    struct NumStartLocationsMap
    {
        // The key is the total number of start locations for the map.
        std::map<int, MapHashMap> val;

        // The key is whether the value is about wins (i.e. true) or losses (i.e. false).
        std::map<bool, std::map<StratSettings, int> > numOutcomes;

        int gameIDIfWonLastGame = -1;
        int gameIDIfLostLastGame = -1;
    };

    struct EnemyRaceScoutedMap
    {
        // The key is enemyRaceInit if the enemy race is known at the start of the game,
        // otherwise if the enemy race is eventually scouted it is the enemy race scouted,
        // otherwise it is empty.
        std::map<std::string, NumStartLocationsMap> val;

        // The key is whether the value is about wins (i.e. true) or losses (i.e. false).
        std::map<bool, std::map<StratSettings, int> > numOutcomes;

        int gameIDIfWonLastGame = -1;
        int gameIDIfLostLastGame = -1;
    };

    struct EnemyRaceInitMap
    {
        // The key is the enemy race at the start of the game (which may be Unknown).
        std::map<std::string, EnemyRaceScoutedMap> val;

        // The key is whether the value is about wins (i.e. true) or losses (i.e. false).
        std::map<bool, std::map<StratSettings, int> > numOutcomes;

        int gameIDIfWonLastGame = -1;
        int gameIDIfLostLastGame = -1;
    };

    struct LearningMap
    {
        EnemyRaceInitMap enemyRaceInitMap;

        // The key is whether the value is about wins (i.e. true) or losses (i.e. false).
        std::map<bool, int> numOutcomes;

        int gameIDIfWonLastGame = -1;
        int gameIDIfLostLastGame = -1;
    } learningMap;
};
