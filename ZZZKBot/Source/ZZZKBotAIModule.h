// Copyright 2020 Chris Coxe.
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
#include <random>

// Reminder: don't use "Broodwar" in any global class constructor!

class ZZZKBotAIModule : public BWAPI::AIModule
{
public:
    // Virtual functions for callbacks, leave these as they are.
    virtual void onStart() override;
    virtual void onEnd(bool isWinner) override;
    virtual void onFrame() override;
    virtual void onSendText(std::string text) override;
    virtual void onReceiveText(BWAPI::Player player, std::string text) override;
    virtual void onPlayerLeft(BWAPI::Player player) override;
    virtual void onNukeDetect(BWAPI::Position target) override;
    virtual void onUnitDiscover(BWAPI::Unit unit) override;
    virtual void onUnitEvade(BWAPI::Unit unit) override;
    virtual void onUnitShow(BWAPI::Unit unit) override;
    virtual void onUnitHide(BWAPI::Unit unit) override;
    virtual void onUnitCreate(BWAPI::Unit unit) override;
    virtual void onUnitDestroy(BWAPI::Unit unit) override;
    virtual void onUnitMorph(BWAPI::Unit unit) override;
    virtual void onUnitRenegade(BWAPI::Unit unit) override;
    virtual void onSaveGame(std::string gameName) override;
    virtual void onUnitComplete(BWAPI::Unit unit) override;
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

    std::set<BWAPI::TilePosition> startLocs;
    BWAPI::TilePosition myStartLoc = BWAPI::TilePositions::Unknown;
    BWAPI::Position myStartRoughPos = BWAPI::Positions::Unknown;
    std::set<BWAPI::TilePosition> otherStartLocs;
    std::set<BWAPI::TilePosition> scoutedOtherStartLocs;
    std::set<BWAPI::TilePosition> unscoutedOtherStartLocs;
    BWAPI::Unit mainBase = nullptr;
    BWAPI::Unit scoutingWorker = nullptr;
    BWAPI::Unit scoutingZergling = nullptr;
    BWAPI::Unit geyser = nullptr;
    bool isScoutingInitialized = false;
    bool isScoutingWorkerReadyToScout = false;
    bool isScoutingUsingWorker = false;
    bool isScoutingUsingZergling = false;
    bool isNeedScoutingWorker = false;
    bool isNeedToMorphScoutingWorker = false;
    std::map<const BWAPI::Unit, BWAPI::Unit> gathererToResourceMap;
    std::map<const BWAPI::Unit, BWAPI::Unit> resourceToGathererMap;

    BWAPI::Unit groundArmyBuildingBuilder = nullptr;
    BWAPI::TilePosition groundArmyBuildingLoc = BWAPI::TilePositions::None;
    int frameLastCheckedGroundArmyBuildingLoc = 0;
    BWAPI::Unit extractorBuilder = nullptr;
    BWAPI::TilePosition extractorLoc = BWAPI::TilePositions::None;
    int frameLastCheckedExtractorLoc = 0;
    BWAPI::Unit creepColonyBuilder = nullptr;
    BWAPI::TilePosition creepColonyLoc = BWAPI::TilePositions::None;
    int frameLastCheckedCreepColonyLoc = 0;
    BWAPI::Unit hatcheryBuilder = nullptr;
    BWAPI::TilePosition hatcheryLoc = BWAPI::TilePositions::None;
    int frameLastCheckedHatcheryLoc = 0;
    BWAPI::Unit queensNestBuilder = nullptr;
    BWAPI::TilePosition queensNestLoc = BWAPI::TilePositions::None;
    int frameLastCheckedQueensNestLoc = 0;
    BWAPI::Unit spireBuilder = nullptr;
    BWAPI::TilePosition spireLoc = BWAPI::TilePositions::None;
    int frameLastCheckedSpireLoc = 0;
    BWAPI::Unit hydraDenBuilder = nullptr;
    BWAPI::TilePosition hydraDenLoc = BWAPI::TilePositions::None;
    int frameLastCheckedHydraDenLoc = 0;
    BWAPI::Unit ultraCavernBuilder = nullptr;
    BWAPI::TilePosition ultraCavernLoc = BWAPI::TilePositions::None;
    int frameLastCheckedUltraCavernLoc = 0;
    int lastAddedGathererToRefinery = 0;
    bool issuedMorphLairCmd = false;
    bool issuedMorphSunkenColonyCmd = false;
    bool issuedMorphHiveCmd = false;
    bool issuedMorphGreaterSpireCmd = false;
    int lastIssuedBuildSupplyProviderCmd = 0;

    // TODO: this bot is currently only designed to support 1v1 games without other players unless they
    // haven't had any buildings and don't currently have any buildings, i.e. these types of players are
    // not currently dealt with properly:
    // allies that are still playing,
    // allies that have left,
    // players that were enemies but have left (i.e. are now neutral not enemies),
    // other neutral players that have neutral buildings.
    bool isARemainingEnemyZerg = false;
    bool isARemainingEnemyTerran = false;
    bool isARemainingEnemyProtoss = false;
    bool isARemainingEnemyRandomRace = false;

    std::set<int> playerIDsLeft;

    bool checkedEnemyDetails = false;
    BWAPI::Race enemyRaceInit;
    BWAPI::Race enemyRaceScouted;
    BWAPI::PlayerType enemyPlayerType;
    int enemyPlayerID = -1;
    std::string enemyName;
    std::string enemyNameUpperCase;
    std::string enemyFileName;
    std::string enemyReadFilePath;
    std::string enemyWriteFilePath;
    BWAPI::TilePosition enemyStartLoc;
    BWAPI::TilePosition enemyStartLocDeduced = BWAPI::TilePositions::Unknown;
    std::set<BWAPI::TilePosition> enemyStartLocs;
    std::set<BWAPI::TilePosition> possibleOverlordScoutLocs;
    BWAPI::TilePosition probableEnemyStartLoc = BWAPI::TilePositions::Unknown;
    std::set<BWAPI::Position> lastKnownEnemyUnliftedBuildingsAnywherePosSet;
    // TODO: add separate logic for enemy overlords (because e.g. on maps with 3 or more start locations
    // the first enemy overlord I see is not necessarily from the start position
    // nearest it).
    BWAPI::Position firstEnemyNonWorkerSeenPos = BWAPI::Positions::Unknown;
    BWAPI::Position closestEnemySeenPos = BWAPI::Positions::Unknown;
    BWAPI::Position furthestEnemySeenPos = BWAPI::Positions::Unknown;
    bool isClosestEnemySeenAnOverlord = false;

    // Each element of the set is a tile position where creep should be on frame zero for that start location.
    struct InitialCreepLocsSet { std::set<BWAPI::TilePosition> val; };
    // The key is the starting location (mine and possible enemy start locations).
    struct InitialCreepLocsMap { std::map<const BWAPI::TilePosition, InitialCreepLocsSet> val; };
    InitialCreepLocsMap initialCreepLocsMap;
    bool isMapPlasma_v_1_0 = false;

    const std::time_t timerAtGameStart = std::time(nullptr);

    int onEndFrameCount = -1;

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
            // Strict weak ordering is required.
            return false;
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

    template <typename RealType = double>
    class beta_distribution
    {
    public:
        typedef RealType result_type;

        class param_type
        {
        public:
            typedef beta_distribution distribution_type;

            explicit param_type(RealType a = 2.0, RealType b = 2.0)
                : a_param(a), b_param(b) { }

            RealType a() const { return a_param; }
            RealType b() const { return b_param; }

            bool operator==(const param_type& other) const
            {
                return (a_param == other.a_param &&
                        b_param == other.b_param);
            }

            bool operator!=(const param_type& other) const
            {
                return !(*this == other);
            }

        private:
            RealType a_param, b_param;
        };

        explicit beta_distribution(RealType a = 2.0, RealType b = 2.0)
            : a_gamma(a), b_gamma(b) { }
        explicit beta_distribution(const param_type& param)
            : a_gamma(param.a()), b_gamma(param.b()) { }

        void reset() { }

        param_type param() const
        {
            return param_type(a(), b());
        }

        void param(const param_type& param)
        {
            a_gamma = gamma_dist_type(param.a());
            b_gamma = gamma_dist_type(param.b());
        }

        template <typename URNG>
        result_type operator()(URNG& engine)
        {
            return generate(engine, a_gamma, b_gamma);
        }

        template <typename URNG>
        result_type operator()(URNG& engine, const param_type& param)
        {
            gamma_dist_type a_param_gamma(param.a()),
                            b_param_gamma(param.b());
            return generate(engine, a_param_gamma, b_param_gamma);
        }

        result_type min() const { return 0.0; }
        result_type max() const { return 1.0; }

        RealType a() const { return a_gamma.alpha(); }
        RealType b() const { return b_gamma.alpha(); }

        bool operator==(const beta_distribution<result_type>& other) const
        {
            return (param() == other.param() &&
                    a_gamma == other.a_gamma &&
                    b_gamma == other.b_gamma);
        }

        bool operator!=(const beta_distribution<result_type>& other) const
        {
            return !(*this == other);
        }

    private:
        typedef std::gamma_distribution<result_type> gamma_dist_type;

        gamma_dist_type a_gamma, b_gamma;

        template <typename URNG>
        result_type generate(URNG& engine,
            gamma_dist_type& x_gamma,
            gamma_dist_type& y_gamma)
        {
            result_type x = x_gamma(engine);
            return x / (x + y_gamma(engine));
        }
    };
};