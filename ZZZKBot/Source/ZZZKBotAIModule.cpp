// Copyright 2015 Chris Coxe.
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
// ExampleAIModule/Source/ExampleAIModule.cpp
// of BWAPI version 4.1.2
// (https://github.com/bwapi/bwapi/releases/tag/v4.1.2 which is
// distributed under the terms of the GNU Lesser General Public License
// (LGPL) version 3).

#include "ZZZKBotAIModule.h"
#include <iostream>
#include <limits>

using namespace BWAPI;
using namespace Filter;

void ZZZKBotAIModule::onStart()
{
    // Print the map name.
    // BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
    Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

    // Enable the UserInput flag, which allows us to control the bot and type messages.
    Broodwar->enableFlag(Flag::UserInput);

    // Uncomment the following line and the bot will know about everything through the fog of war (cheat).
    //Broodwar->enableFlag(Flag::CompleteMapInformation);

    // Set the command optimization level so that common commands can be grouped
    // and reduce the bot's APM (Actions Per Minute).
    //Broodwar->setCommandOptimizationLevel(2);
    Broodwar->setCommandOptimizationLevel(1);

    // Speedups (including disabling the GUI) for automated play.
    //BWAPI::Broodwar->setLocalSpeed(0);
    //BWAPI::Broodwar->setFrameSkip(16);   // Not needed if using setGUI(false).
    //BWAPI::Broodwar->setGUI(false);

    // Check if this is a replay
    if (Broodwar->isReplay())
    {
        // Announce the players in the replay
        Broodwar << "The following players are in this replay:" << std::endl;

        // Iterate all the players in the game using a std:: iterator
        Playerset players = Broodwar->getPlayers();
        for (auto p : players)
        {
            // Only print the player if they are not an observer
            if (!p->isObserver())
            {
                Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
            }
        }
    }
    else // if this is not a replay
    {
        // Retrieve you and your enemy's races. enemy() will just return the first enemy.
        // If you wish to deal with multiple enemies then you must use enemies().
        if (Broodwar->enemy()) // First make sure there is an enemy
        {
            Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
        }
    }
}

void ZZZKBotAIModule::onEnd(bool isWinner)
{
    // Called when the game ends
    if (isWinner)
    {
        // Log your win here!
    }
}

void ZZZKBotAIModule::onFrame()
{
    // Called once every game frame

    // Display the game frame rate as text in the upper left area of the screen
    Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
    Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS() );

    // Return if the game is a replay or is paused
    if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
    {
        return;
    }

    // Unit client info indices.
    // The argument of getClientInfo() is the index.
    // The arguments of setClientInfo() are the value then the index.
    // Note: querying for a value that doesn't exist will return 0,
    // so you can't distinguish value 0 from undefined, so be careful about
    // storing zeroed values.
    // Let's use value 1 to mean it is (i.e. is currently, or was when we check it in later frames)
    // carrying minerals and 0 means not.
    const int wasJustCarryingMineralsInd = 1;
    const int wasJustCarryingMineralsDefaultVal = 0;
    const int wasJustCarryingMineralsTrueVal = 1;
    const int frameLastChangedPosInd = 2;
    const int frameLastAttackingInd = 3;
    const int frameLastAttackFrameInd = 4;
    const int frameLastStartingAttackInd = 5;
    const int frameLastStoppedInd = 6;
    const int posXInd = 7;
    const int posYInd = 8;
    const int scoutingTargetPosXInd = 9;
    const int scoutingTargetPosYInd = 10;

    static BWAPI::Unit startBase = nullptr;
    static std::map<const BWAPI::Unit, BWAPI::Unit> gathererToResourceMap;
    auto gathererToResourceMapAuto = gathererToResourceMap;
    static std::map<const BWAPI::Unit, BWAPI::Unit> resourceToGathererMap;
    auto resourceToGathererMapAuto = resourceToGathererMap;

    if (startBase == nullptr || !startBase->exists())
    {
        startBase = BWAPI::Broodwar->getClosestUnit(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()), BWAPI::Filter::IsResourceDepot && BWAPI::Filter::IsOwned && BWAPI::Filter::IsCompleted && !BWAPI::Filter::IsLifted && BWAPI::Filter::Exists);
    }

    auto startBaseAuto = startBase;

    // Converts a specified BWAPI::TilePosition and building type into a BWAPI::Position that would be roughly at the
    // centre of the building if it is built at the specified BWAPI::TilePosition.
    auto getPos =
        [](const BWAPI::TilePosition tp, const BWAPI::UnitType ut)
        {
            return Position(Position(tp) + Position((ut.tileWidth() * BWAPI::TILEPOSITION_SCALE) / 2, (ut.tileHeight() * BWAPI::TILEPOSITION_SCALE) / 2));
        };

    static std::set<BWAPI::Position> enemyStartPositions;
    static std::set<BWAPI::Position> possibleOverlordScoutPositions;
    for (const BWAPI::Player& p : Broodwar->enemies())
    {
        const BWAPI::TilePosition enemyStartLocation = p->getStartLocation();
        if (enemyStartLocation != BWAPI::TilePositions::Unknown && enemyStartLocation != BWAPI::TilePositions::None)
        {
            const BWAPI::Position pos = getPos(enemyStartLocation, BWAPI::UnitTypes::Special_Start_Location);
            if (enemyStartPositions.empty())
            {
                possibleOverlordScoutPositions.clear();
            }

            possibleOverlordScoutPositions.insert(pos);
            enemyStartPositions.insert(pos);
        }
    }

    static std::set<BWAPI::Position> startPositions;
    static BWAPI::Position myStartPos = BWAPI::Positions::Unknown;
    static std::set<BWAPI::Position> otherStartPositions;
    static std::set<BWAPI::Position> scoutedOtherStartPositions;
    static std::set<BWAPI::Position> unscoutedOtherStartPositions;
    for (const BWAPI::TilePosition tp : Broodwar->getStartLocations())
    {
        startPositions.insert(getPos(tp, BWAPI::UnitTypes::Special_Start_Location));

        if (tp == Broodwar->self()->getStartLocation())
        {
            if (myStartPos == BWAPI::Positions::Unknown)
            {
                myStartPos = getPos(tp, BWAPI::UnitTypes::Special_Start_Location);
            }
        }
        else
        {
            const std::pair<std::set<BWAPI::Position>::iterator, bool> ret =
                otherStartPositions.insert(getPos(tp, BWAPI::UnitTypes::Special_Start_Location));

            if (ret.second)
            {
                unscoutedOtherStartPositions.insert(*ret.first);
                if (enemyStartPositions.empty())
                {
                    possibleOverlordScoutPositions.insert(*ret.first);
                }
            }
        }
    }

    for (const BWAPI::Position otherStartPos : unscoutedOtherStartPositions)
    {
        if (Broodwar->isVisible(TilePosition(otherStartPos)) &&
            Broodwar->getUnitsOnTile(TilePosition(otherStartPos), IsEnemy && IsVisible && Exists && IsBuilding && !IsLifted).empty())
        {
            scoutedOtherStartPositions.insert(otherStartPos);
        }
    }

    for (const BWAPI::Position otherStartPos : scoutedOtherStartPositions)
    {
        unscoutedOtherStartPositions.erase(otherStartPos);
    }

    const std::string& enemyName = Broodwar->enemy() ? Broodwar->enemy()->getName() : "";
    std::string enemyNameUpperCase = enemyName;
    std::transform(enemyNameUpperCase.begin(), enemyNameUpperCase.end(),enemyNameUpperCase.begin(), ::toupper);

    static bool isSpeedlingBuildOrder =
        enemyNameUpperCase.compare(0, std::string("XIMP").size(), "XIMP") == 0 ||
        enemyNameUpperCase.compare(0, std::string("OVERKILL").size(), "OVERKILL") == 0 ||
        enemyNameUpperCase.compare(0, std::string("UALBERTA").size(), "UALBERTA") == 0 ||
        // Because I have renamed it to "UAB" case-sensitive (was "UAlbertaBot" case-sensitive).
        enemyNameUpperCase.compare(0, std::string("UAB").size(), "UAB") == 0 ||
        enemyNameUpperCase.compare(0, std::string("GARM").size(), "GARM") == 0 ||
        enemyNameUpperCase.compare(0, std::string("TYR").size(), "TYR") == 0;

    static bool isEnemyXimp =
        enemyNameUpperCase.compare(0, std::string("XIMP").size(), "XIMP") == 0;

    const int transitionOutOfFourPoolFrameCountThresh = isSpeedlingBuildOrder ? 0 : (15 * 60 * 24);

    // We ignore stolen gas, at least until a time near when we plan to make an extractor.
    auto isNotStolenGas =
        [&startBaseAuto, &transitionOutOfFourPoolFrameCountThresh](const Unit& tmpUnit)
        {
            return
                !tmpUnit->getType().isRefinery() ||
                startBaseAuto == nullptr ||
                Broodwar->getFrameCount() + (60 * 24) >= transitionOutOfFourPoolFrameCountThresh ||
                startBaseAuto->getDistance(tmpUnit) > 256;
        };

    static std::set<BWAPI::Position> lastKnownEnemyUnliftedBuildingsAnywherePosSet;
    // Block to restrict scope of variables.
    {
        std::set<BWAPI::Position> vacantPosSet;
        for (const BWAPI::Position pos : lastKnownEnemyUnliftedBuildingsAnywherePosSet)
        {
            if (Broodwar->isVisible(TilePosition(pos)) &&
                Broodwar->getUnitsOnTile(
                    TilePosition(pos),
                    IsEnemy && IsVisible && Exists && IsBuilding && !IsLifted &&
                    isNotStolenGas).empty())
            {
                vacantPosSet.insert(pos);
            }
        }

        for (const BWAPI::Position pos : vacantPosSet)
        {
            lastKnownEnemyUnliftedBuildingsAnywherePosSet.erase(pos);
        }
    }

    // TODO: add separate logic for enemy overlords (because e.g. on maps with 3 or more start locations
    // the first enemy overlord I see is not necessarily from the start position
    // nearest it).
    static BWAPI::Position firstEnemyNonWorkerSeenPos = BWAPI::Positions::Unknown;
    static BWAPI::Position closestEnemySeenPos = BWAPI::Positions::Unknown;
    static BWAPI::Position furthestEnemySeenPos = BWAPI::Positions::Unknown;

    const Unitset& allUnits = Broodwar->getAllUnits();
    for (auto& u : allUnits)
    {
        if (u->exists() && u->isVisible() && u->getPlayer() && u->getPlayer()->isEnemy(Broodwar->self()))
        {
            if (u->getType().isBuilding() && !u->isLifted() && isNotStolenGas(u))
            {
                lastKnownEnemyUnliftedBuildingsAnywherePosSet.insert(u->getPosition());
            }

            if (myStartPos != BWAPI::Positions::Unknown &&
                firstEnemyNonWorkerSeenPos == BWAPI::Positions::Unknown &&
                !u->getType().isWorker() /*&&
                u->getType() != BWAPI::UnitTypes::Zerg_Overlord*/)
            {
                if (closestEnemySeenPos == BWAPI::Positions::Unknown || myStartPos.getDistance(u->getPosition()) < myStartPos.getDistance(closestEnemySeenPos))
                {
                    closestEnemySeenPos = u->getPosition();
                }

                if (furthestEnemySeenPos == BWAPI::Positions::Unknown || myStartPos.getDistance(u->getPosition()) > myStartPos.getDistance(furthestEnemySeenPos))
                {
                    furthestEnemySeenPos = u->getPosition();
                }
            }
        }
    }

    BWAPI::Position probableEnemyStartPos = BWAPI::Positions::Unknown;
    if (firstEnemyNonWorkerSeenPos == BWAPI::Positions::Unknown && furthestEnemySeenPos != BWAPI::Positions::Unknown)
    {
        firstEnemyNonWorkerSeenPos = furthestEnemySeenPos;
    }

    if (furthestEnemySeenPos != BWAPI::Positions::Unknown)
    {
        for (const BWAPI::Position pos : unscoutedOtherStartPositions)
        {
            if (probableEnemyStartPos == BWAPI::Positions::Unknown || furthestEnemySeenPos.getDistance(pos) < furthestEnemySeenPos.getDistance(probableEnemyStartPos))
            {
                probableEnemyStartPos = pos;
            }
        }
    }

    if (closestEnemySeenPos != BWAPI::Positions::Unknown && probableEnemyStartPos != BWAPI::Positions::Unknown)
    {
        if (myStartPos != BWAPI::Positions::Unknown && myStartPos.getDistance(closestEnemySeenPos) < probableEnemyStartPos.getDistance(closestEnemySeenPos))
        {
            // We send combat units to other starting positions in order of their closeness,
            // and 4pool should get combat units faster than any other build, so on most maps
            // our first 6 lings should see their combat unit before it gets closer to us than
            // we are to them if they are at one of the two closest other start positions.
            // If not then don't try to guess where they are. TODO: This logic doesn't always
            // work if the enemy made proxy rax/gateway though.
            probableEnemyStartPos = BWAPI::Positions::Unknown;
        }
    }

    if (Broodwar->getLatencyFrames() != 2 && Broodwar->getFrameCount() % 2 != 1)
    {
        return;
    }

    const Unitset& myUnits = Broodwar->self()->getUnits();

    // For some reason supplyUsed() takes a few frames get adjusted after an extractor starts morphing,
    // so count it myself, but don't count it myself after I start getting gas because I assume the worker
    // currently in the extractor could be missed.
    int supplyUsed = 0;

    // Special logic to find an enemy unit to attack if there are multiple
    // enemy non-building units (or a single SCV) near any of our workers (e.g. an
    // enemy worker rush) and at least one enemy unit is in one of our worker's
    // weapon range, or at least one of our buildings is low health.
    Unitset myCompletedWorkers;
    BWAPI::Unit lowLifeDrone = nullptr;
    bool isBuildingLowLife = false;

    // Count units by type myself because Broodwar->self()->allUnitCount() etc does
    // not count the unit(s) within eggs/lurker eggs/cocoons. Notes:
    // Hydras/mutalisks that are morphing into lurkers/guardians/devourers are only
    // counted as the incomplete type they are morphing to (the count for the type
    // they are morphing from is not increased).
    // The counts might not count the worker currently inside the extractor, if any.
    // Eggs, lurker eggs and cocoons have their own count (in addition to counting
    // what they contain).
    std::map<const BWAPI::UnitType, int> allUnitCount;
    std::map<const BWAPI::UnitType, int> incompleteUnitCount;
    std::map<const BWAPI::UnitType, int> completedUnitCount;

    std::map<const BWAPI::Position, int> numUnitsTargetingPos;

    for (auto& u : myUnits)
    {
        if (!u->exists())
        {
            continue;
        }

        ++allUnitCount[u->getType()];
        if (u->isCompleted())
        {
            ++completedUnitCount[u->getType()];
        }
        else
        {
            ++incompleteUnitCount[u->getType()];
        }

        supplyUsed += u->getType().supplyRequired();
        if (u->getType() == BWAPI::UnitTypes::Zerg_Egg || u->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg || u->getType() == BWAPI::UnitTypes::Zerg_Cocoon)
        {
            const BWAPI::UnitType buildType = u->getBuildType();
            if (buildType != BWAPI::UnitTypes::None &&
                buildType != BWAPI::UnitTypes::Unknown)
            {
                int tmpCount = buildType.isTwoUnitsInOneEgg() ? 2 : 1;
                allUnitCount[buildType] += tmpCount;
                incompleteUnitCount[buildType] += tmpCount;
                supplyUsed += buildType.supplyRequired() * tmpCount;
            }
        }

        if (u->getType().isWorker() && u->isCompleted())
        {
            myCompletedWorkers.insert(u);
        }

        if (u->getType() == BWAPI::UnitTypes::Zerg_Drone && u->getHitPoints() <= 10)
        {
            // Don't interrupt its attack if it is attacking.
            if (u->getLastCommand().getType() == BWAPI::UnitCommandTypes::None ||
                Broodwar->getFrameCount() >= u->getLastCommandFrame() + 2 - (Broodwar->getLatencyFrames() > 2 ? u->getLastCommandFrame() % 2 : 0) ||
                !(u->getTarget() && u->getTarget()->getPlayer() && u->getTarget()->getPlayer()->isEnemy(Broodwar->self())))
            {
                lowLifeDrone = u;
            }
        }

        if (u->getType().isBuilding() && u->isCompleted() &&
            u->getHitPoints() + u->getShields() < ((u->getType().maxHitPoints() + u->getType().maxShields()) * 3) / 10)
        {
            isBuildingLowLife = true;
        }

        if (u->getType() != BWAPI::UnitTypes::Zerg_Overlord)
        {
            const int tmpX = (int) u->getClientInfo(scoutingTargetPosXInd);
            const int tmpY = (int) u->getClientInfo(scoutingTargetPosYInd);
            if (tmpX != 0 || tmpY != 0)
            {
                ++numUnitsTargetingPos[Position(tmpX, tmpY)];
            }
        }
    }

    if (Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh || supplyUsed >= 60)
    {
        supplyUsed = Broodwar->self()->supplyUsed();
    }

    // Worker/base defence logic.
    bool workersShouldRetaliate = false;
    bool shouldDefend = false;
    BWAPI::Unit workerAttackTargetUnit = nullptr;
    for (auto& u : myCompletedWorkers)
    {
        const Unitset& attackableEnemyNonBuildingThreatUnits =
            u->getUnitsInRadius(
                256,
                IsEnemy && IsVisible && IsDetected && Exists &&
                CanAttack &&
                !IsBuilding &&
                [&u, &startBaseAuto](Unit& tmpUnit)
                {
                    return (startBaseAuto ? startBaseAuto->getDistance(tmpUnit) < 256 : true) && u->canAttack(PositionOrUnit(tmpUnit));
                });

        if (isBuildingLowLife ||
            attackableEnemyNonBuildingThreatUnits.size() >= 2 ||
            (myCompletedWorkers.size() > 1 &&
             // Note: using allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] rather than incompleteUnitCount[BWAPI::UnitTypes::Zerg_Extractor]
             // because BWAPI seems to think it is completed.
             myCompletedWorkers.size() + allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] + allUnitCount[BWAPI::UnitTypes::Zerg_Spawning_Pool] < 4))
        {
            workersShouldRetaliate = true;
            shouldDefend = true;
        }
        else if (!attackableEnemyNonBuildingThreatUnits.empty() &&
                 (*attackableEnemyNonBuildingThreatUnits.begin())->getType() == BWAPI::UnitTypes::Terran_SCV)
        {
            workersShouldRetaliate = true;
        }

        if (workersShouldRetaliate)
        {
            const BWAPI::Unit& tmpEnemyUnit =
                Broodwar->getBestUnit(
                    [&u](const BWAPI::Unit& bestSoFarUnit, const BWAPI::Unit& curUnit)
                    {
                        if (u->isInWeaponRange(curUnit) != u->isInWeaponRange(bestSoFarUnit))
                        {
                            return u->isInWeaponRange(curUnit) ? curUnit : bestSoFarUnit;
                        }

                        return curUnit->getHitPoints() + curUnit->getShields() + curUnit->getType().armor() + curUnit->getDefenseMatrixPoints() < bestSoFarUnit->getHitPoints() + bestSoFarUnit->getShields() + bestSoFarUnit->getType().armor() + bestSoFarUnit->getDefenseMatrixPoints() ? curUnit : bestSoFarUnit;
                    },
                    IsEnemy && IsVisible && IsDetected && Exists &&
                    CanAttack &&
                    !IsBuilding &&
                    [&u, &startBaseAuto, &shouldDefend](Unit& tmpUnit)
                    {
                        return
                            (shouldDefend ?
                             (startBaseAuto ? startBaseAuto->getDistance(tmpUnit) < 256 : true) :
                             u->isInWeaponRange(tmpUnit)) &&
                            u->canAttack(PositionOrUnit(tmpUnit));
                    },
                    u->getPosition(),
                    std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + 256);

            if (tmpEnemyUnit != nullptr)
            {
                if (workerAttackTargetUnit == nullptr ||
                    tmpEnemyUnit->getHitPoints() + tmpEnemyUnit->getShields() + tmpEnemyUnit->getType().armor() + tmpEnemyUnit->getDefenseMatrixPoints() < workerAttackTargetUnit->getHitPoints() + workerAttackTargetUnit->getShields() + workerAttackTargetUnit->getType().armor() + workerAttackTargetUnit->getDefenseMatrixPoints())
                {
                    workerAttackTargetUnit = tmpEnemyUnit;
                }
            }
        }
    }

    if (workerAttackTargetUnit == nullptr && isBuildingLowLife)
    {
        for (auto& u : myUnits)
        {
            if (u->exists() && u->getType().isBuilding() && u->isCompleted() &&
                u->getHitPoints() + u->getShields() < ((u->getType().maxHitPoints() + u->getType().maxShields()) * 3) / 10)
            {
                workerAttackTargetUnit =
                    u->getClosestUnit(
                        IsEnemy && IsVisible && IsDetected && Exists &&
                        CanAttack &&
                        !IsBuilding &&
                        !IsFlying &&
                        !IsInvincible,
                        256);

                if (workerAttackTargetUnit != nullptr)
                {
                    break;
                }
            }
        }
    }

    // Checks whether BWAPI already has a command pending to be executed for the specified unit.
    auto noCmdPending =
        [](const BWAPI::Unit& tmpUnit)
        {
            return
                (bool)
                (tmpUnit->getLastCommand().getType() == BWAPI::UnitCommandTypes::None ||
                 Broodwar->getFrameCount() >= tmpUnit->getLastCommandFrame() + (Broodwar->getLatencyFrames() > 2 ? Broodwar->getLatencyFrames() - (tmpUnit->getLastCommandFrame() % 2) : Broodwar->getLatencyFrames()));
        };

    // Logic to make a building.
    // TODO: support making buildings concurrently (rather than designing each building's prerequisites to avoid this situation).
    // TODO: support making more than one building of a particular type.
    // Note: geyser is only used when building an extractor.
    static BWAPI::Unit geyser = nullptr;
    auto geyserAuto = geyser;
    auto makeUnit =
        [&startBaseAuto, &allUnitCount, &getPos, &gathererToResourceMapAuto, &resourceToGathererMapAuto, &lowLifeDrone, &geyserAuto, &noCmdPending](
            const BWAPI::UnitType& buildingType,
            BWAPI::Unit& reservedBuilder,
            BWAPI::TilePosition& targetBuildLocation,
            int& frameLastCheckedBuildLocation,
            const int checkBuildLocationFreqFrames,
            const bool isNeeded)
        {
            BWAPI::UnitType builderType = buildingType.whatBuilds().first;

            // TODO: support making more than one building of a particular type.
            if ((allUnitCount[buildingType] > 0 && buildingType != BWAPI::UnitTypes::Zerg_Hatchery) ||
                (reservedBuilder &&
                 (!reservedBuilder->exists() ||
                  reservedBuilder->getType() != builderType)))
            {
                reservedBuilder = nullptr;
            }

            BWAPI::Unit oldReservedBuilder = reservedBuilder;
            const int oldUnitCount = allUnitCount[buildingType];
        
            // TODO: support making more than one building of a particular type.
            if (allUnitCount[buildingType] == 0 && isNeeded)
            {
                BWAPI::Unit builder = reservedBuilder;
                reservedBuilder = nullptr;

                if (buildingType == BWAPI::UnitTypes::Zerg_Extractor && lowLifeDrone)
                {
                    builder = lowLifeDrone;
                }

                auto isAvailableToBuild =
                    [&startBaseAuto, &noCmdPending](Unit& tmpUnit)
                    {
                        return !tmpUnit->isConstructing() && noCmdPending(tmpUnit);
                    };

                if (builder == nullptr && startBaseAuto)
                {        
                    builder = startBaseAuto->getClosestUnit(GetType == builderType && IsIdle && !IsCarryingSomething && IsOwned && isAvailableToBuild);
                    if (builder == nullptr)
                        builder = startBaseAuto->getClosestUnit(GetType == builderType && IsGatheringMinerals && !IsCarryingSomething && IsOwned && isAvailableToBuild);
                    // In case we are being worker rushed, don't necessarily wait for workers to return their
                    // minerals/gas powerup because we should start building the pool asap and the workers are
                    // likely to be almost always fighting.
                    if (buildingType == BWAPI::UnitTypes::Zerg_Spawning_Pool)
                    {
                        if (builder == nullptr)
                            builder = startBaseAuto->getClosestUnit(GetType == builderType && IsIdle && !IsCarryingGas && IsOwned && isAvailableToBuild);
                        if (builder == nullptr)
                            builder = startBaseAuto->getClosestUnit(GetType == builderType && IsGatheringMinerals && IsOwned && isAvailableToBuild);
                        if (builder == nullptr)
                            builder = startBaseAuto->getClosestUnit(GetType == builderType && IsGatheringGas && !IsCarryingGas && IsOwned && isAvailableToBuild);
                        if (builder == nullptr)
                            builder = startBaseAuto->getClosestUnit(GetType == builderType && IsGatheringGas && IsOwned && isAvailableToBuild);
                    }
                }

                // If a unit was found
                if (builder && (builder != oldReservedBuilder || isAvailableToBuild(builder)))
                {
                    if (targetBuildLocation == BWAPI::TilePositions::None ||
                        targetBuildLocation == BWAPI::TilePositions::Unknown ||
                        targetBuildLocation == BWAPI::TilePositions::Invalid ||
                        Broodwar->getFrameCount() >= frameLastCheckedBuildLocation + checkBuildLocationFreqFrames)
                    {
                        if (buildingType == BWAPI::UnitTypes::Zerg_Extractor && startBaseAuto)
                        {
                            geyserAuto =
                                startBaseAuto->getClosestUnit(
                                    GetType == BWAPI::UnitTypes::Resource_Vespene_Geyser &&
                                    BWAPI::Filter::Exists,
                                    256);
        
                            if (geyserAuto)
                            {
                                targetBuildLocation = geyserAuto->getTilePosition();
                            }
                        }
                        else
                        {
                            targetBuildLocation = Broodwar->getBuildLocation(buildingType, builder->getTilePosition());
                        }

                        frameLastCheckedBuildLocation = Broodwar->getFrameCount();
                    }

                    if (targetBuildLocation != BWAPI::TilePositions::None &&
                        targetBuildLocation != BWAPI::TilePositions::Unknown &&
                        targetBuildLocation != BWAPI::TilePositions::Invalid)
                    {
                        if (builder->canBuild(buildingType))
                        {
                            if (builder->canBuild(buildingType, targetBuildLocation) &&
                                (buildingType != BWAPI::UnitTypes::Zerg_Extractor ||
                                 (geyserAuto &&
                                  geyserAuto->exists() &&
                                  geyserAuto->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)))
                            {
                                if (buildingType == BWAPI::UnitTypes::Zerg_Extractor &&
                                    builder->canRightClick(PositionOrUnit(geyserAuto)) &&
                                    builder->getDistance(geyserAuto) > 16)
                                {
                                    builder->rightClick(geyserAuto);
                                }
                                else
                                {
                                    builder->build(buildingType, targetBuildLocation);
                                }

                                reservedBuilder = builder;
                            }
                            else
                            {
                                targetBuildLocation = BWAPI::TilePositions::None;
                                if (buildingType == BWAPI::UnitTypes::Zerg_Extractor)
                                {
                                    geyserAuto = nullptr;
                                }
                            }
                        }
                        // Not enough minerals or it is not available (e.g. UMS game type).
                        else if (buildingType == BWAPI::UnitTypes::Zerg_Spawning_Pool && Broodwar->self()->isUnitAvailable(buildingType))
                        {
                            // Not enough minerals, so send a worker out to the build location so it is on or nearer the
                            // position when we have enough minerals.
                            const Position targetBuildPos = getPos(targetBuildLocation, buildingType);
                            if (builder->canRightClick(PositionOrUnit(targetBuildPos)))
                            {
                                builder->rightClick(targetBuildPos);
                                reservedBuilder = builder;
                            }
                            else if (builder->canMove())
                            {
                                builder->move(targetBuildPos);
                                reservedBuilder = builder;
                            }
                        }

                        if (reservedBuilder != nullptr)
                        {
                            if (gathererToResourceMapAuto.find(reservedBuilder) != gathererToResourceMapAuto.end() && resourceToGathererMapAuto.find(gathererToResourceMapAuto.at(reservedBuilder)) != resourceToGathererMapAuto.end() && resourceToGathererMapAuto.at(gathererToResourceMapAuto.at(reservedBuilder)) == reservedBuilder)
                            {
                                resourceToGathererMapAuto.erase(gathererToResourceMapAuto.at(reservedBuilder));
                            }
    
                            gathererToResourceMapAuto.erase(reservedBuilder);
                        }
                    }
                }
            }

            if (oldReservedBuilder != nullptr &&
                // TODO: support making more than one building of a particular type.
                ((oldUnitCount == 0 && !isNeeded) ||
                 (reservedBuilder != nullptr &&
                  reservedBuilder != oldReservedBuilder)) &&
                oldReservedBuilder->getLastCommand().getType() != BWAPI::UnitCommandTypes::None &&
                noCmdPending(oldReservedBuilder) &&
                oldReservedBuilder->canStop())
            {
                oldReservedBuilder->stop();

                if (gathererToResourceMapAuto.find(oldReservedBuilder) != gathererToResourceMapAuto.end() && resourceToGathererMapAuto.find(gathererToResourceMapAuto.at(oldReservedBuilder)) != resourceToGathererMapAuto.end() && resourceToGathererMapAuto.at(gathererToResourceMapAuto.at(oldReservedBuilder)) == oldReservedBuilder)
                {
                    resourceToGathererMapAuto.erase(gathererToResourceMapAuto.at(oldReservedBuilder));
                }

                gathererToResourceMapAuto.erase(oldReservedBuilder);
            }

            if (oldUnitCount > 0 || !isNeeded)
            {
                reservedBuilder = nullptr;
            }
            else if (reservedBuilder == nullptr)
            {
                reservedBuilder = oldReservedBuilder;
            }
        };

    int numWorkersTrainedThisFrame = 0;

    const BWAPI::UnitType groundArmyUnitType =
        Broodwar->self()->getRace() == Races::Terran ? UnitTypes::Terran_Marine : (Broodwar->self()->getRace() == Races::Protoss ? UnitTypes::Protoss_Zealot : UnitTypes::Zerg_Zergling);

    const UnitType groundArmyBuildingType =
        Broodwar->self()->getRace() == Races::Terran ? UnitTypes::Terran_Barracks : (Broodwar->self()->getRace() == Races::Protoss ? UnitTypes::Protoss_Gateway : UnitTypes::Zerg_Spawning_Pool);

    // We are 4-pool'ing, hence the figure 24 (i.e. start moving a worker to the build location before we have enough minerals).
    // If drone(s) have died then don't move the builder until we have the full amount of minerals required.
    static Unit groundArmyBuildingBuilder = nullptr;
    // Block to restrict scope of variables.
    {
        static BWAPI::TilePosition groundArmyBuildingLocation = BWAPI::TilePositions::None;
        static int frameLastCheckedGroundArmyBuildingLocation = 0;
        const int checkGroundArmyBuildingLocationFreqFrames = (10 * 24);
        makeUnit(
            groundArmyBuildingType, groundArmyBuildingBuilder, groundArmyBuildingLocation, frameLastCheckedGroundArmyBuildingLocation, checkGroundArmyBuildingLocationFreqFrames,
            // Note: using allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] rather than incompleteUnitCount[BWAPI::UnitTypes::Zerg_Extractor]
            // because BWAPI seems to think it is completed.
            allUnitCount[BWAPI::UnitTypes::Zerg_Drone] + numWorkersTrainedThisFrame + allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] >= 4 &&
            (Broodwar->canMake(groundArmyBuildingType) ||
             (groundArmyBuildingType == BWAPI::UnitTypes::Zerg_Spawning_Pool &&
              Broodwar->self()->deadUnitCount(BWAPI::UnitTypes::Zerg_Drone) == 0 &&
              Broodwar->self()->minerals() >= groundArmyBuildingType.mineralPrice() - 24)));
    }

    // Use the extractor trick whenever possible when supply-blocked, or when a drone is very low life,
    // or morph an extractor for gathering gas if the time is right.
    static Unit extractorBuilder = nullptr;
    // Block to restrict scope of variables.
    {
        static BWAPI::TilePosition extractorLocation = BWAPI::TilePositions::None;
        static int frameLastCheckedExtractorLocation = 0;
        const int checkExtractorLocationFreqFrames = (1 * 24);
        makeUnit(
            BWAPI::UnitTypes::Zerg_Extractor, extractorBuilder, extractorLocation, frameLastCheckedExtractorLocation, checkExtractorLocationFreqFrames,
            Broodwar->canMake(BWAPI::UnitTypes::Zerg_Extractor) &&
            (((supplyUsed == Broodwar->self()->supplyTotal() || supplyUsed == Broodwar->self()->supplyTotal() - 1) &&
              Broodwar->self()->minerals() >= 84 &&
              !(Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh || supplyUsed >= 60)) ||
             lowLifeDrone != nullptr ||
             (allUnitCount[BWAPI::UnitTypes::Zerg_Spawning_Pool] > 0 &&
              allUnitCount[BWAPI::UnitTypes::Zerg_Drone] + numWorkersTrainedThisFrame >= 9 &&
              (Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh || supplyUsed >= 60))));
    }

    // Morph another hatchery late-game.
    // Block to restrict scope of variables.
    {
        static Unit hatcheryBuilder = nullptr;
        static BWAPI::TilePosition hatcheryLocation = BWAPI::TilePositions::None;
        static int frameLastCheckedHatcheryLocation = 0;
        const int checkHatcheryLocationFreqFrames = (10 * 24);
        makeUnit(
            BWAPI::UnitTypes::Zerg_Hatchery, hatcheryBuilder, hatcheryLocation, frameLastCheckedHatcheryLocation, checkHatcheryLocationFreqFrames,
            Broodwar->canMake(BWAPI::UnitTypes::Zerg_Hatchery) &&
            allUnitCount[BWAPI::UnitTypes::Zerg_Hatchery] + allUnitCount[BWAPI::UnitTypes::Zerg_Lair] + allUnitCount[BWAPI::UnitTypes::Zerg_Hive] <= 1 &&
            (Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh || supplyUsed >= 60) &&
            (isSpeedlingBuildOrder ? true : (allUnitCount[BWAPI::UnitTypes::Zerg_Lair] + allUnitCount[BWAPI::UnitTypes::Zerg_Hive] > 0)));
    }

    // Morph to queen's nest late-game.
    // Block to restrict scope of variables.
    {
        static Unit queensNestBuilder = nullptr;
        static BWAPI::TilePosition queensNestLocation = BWAPI::TilePositions::None;
        static int frameLastCheckedQueensNestLocation = 0;
        const int checkQueensNestLocationFreqFrames = (10 * 24);
        makeUnit(
            BWAPI::UnitTypes::Zerg_Queens_Nest, queensNestBuilder, queensNestLocation, frameLastCheckedQueensNestLocation, checkQueensNestLocationFreqFrames,
            Broodwar->canMake(BWAPI::UnitTypes::Zerg_Queens_Nest) &&
            (Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh || supplyUsed >= 60) &&
            (isSpeedlingBuildOrder ? allUnitCount[BWAPI::UnitTypes::Zerg_Spire] > 0 : true));
    }

    // Morph to spire late-game. Check that a queen's nest and hive are already morphing/completed
    // because I prefer mutalisks or guardians before hydralisks,
    // and otherwise builders may try to build at the same place and fail.
    // Block to restrict scope of variables.
    {
        static Unit spireBuilder = nullptr;
        static BWAPI::TilePosition spireLocation = BWAPI::TilePositions::None;
        static int frameLastCheckedSpireLocation = 0;
        const int checkSpireLocationFreqFrames = (10 * 24);
        makeUnit(
            BWAPI::UnitTypes::Zerg_Spire, spireBuilder, spireLocation, frameLastCheckedSpireLocation, checkSpireLocationFreqFrames,
            Broodwar->canMake(BWAPI::UnitTypes::Zerg_Spire) &&
            (Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh || supplyUsed >= 60) &&
            allUnitCount[BWAPI::UnitTypes::Zerg_Greater_Spire] == 0 &&
            (isSpeedlingBuildOrder ? true :
             (allUnitCount[BWAPI::UnitTypes::Zerg_Queens_Nest] > 0 &&
              allUnitCount[BWAPI::UnitTypes::Zerg_Hive] > 0)));
    }

    // Morph to hydralisk den late-game. Check that a queen's nest and hive and spire
    // and some guardians are already morphing/completed
    // because I prefer mutalisks or guardians before hydralisks,
    // and otherwise builders may try to build at the same place and fail.
    // Block to restrict scope of variables.
    {
        static Unit hydraDenBuilder = nullptr;
        static BWAPI::TilePosition hydraDenLocation = BWAPI::TilePositions::None;
        static int frameLastCheckedHydraDenLocation = 0;
        const int checkHydraDenLocationFreqFrames = (10 * 24);
        makeUnit(
            BWAPI::UnitTypes::Zerg_Hydralisk_Den, hydraDenBuilder, hydraDenLocation, frameLastCheckedHydraDenLocation, checkHydraDenLocationFreqFrames,
            Broodwar->canMake(BWAPI::UnitTypes::Zerg_Hydralisk_Den) &&
            (Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh || supplyUsed >= 60) &&
            allUnitCount[BWAPI::UnitTypes::Zerg_Queens_Nest] > 0 &&
            allUnitCount[BWAPI::UnitTypes::Zerg_Hive] > 0 &&
            allUnitCount[BWAPI::UnitTypes::Zerg_Spire] + allUnitCount[BWAPI::UnitTypes::Zerg_Greater_Spire] > 0 &&
            allUnitCount[BWAPI::UnitTypes::Zerg_Guardian] > 8);
    }

    // Morph to ultralisk cavern late-game. Check that a hyrdalisk den and greater spire
    // and some guardians are already morphing/completed
    // because I prefer guardians before ultras.
    // Block to restrict scope of variables.
    {
        static Unit ultraCavernBuilder = nullptr;
        static BWAPI::TilePosition ultraCavernLocation = BWAPI::TilePositions::None;
        static int frameLastCheckedUltraCavernLocation = 0;
        const int checkUltraCavernLocationFreqFrames = (10 * 24);
        makeUnit(
            BWAPI::UnitTypes::Zerg_Ultralisk_Cavern, ultraCavernBuilder, ultraCavernLocation, frameLastCheckedUltraCavernLocation, checkUltraCavernLocationFreqFrames,
            Broodwar->canMake(BWAPI::UnitTypes::Zerg_Ultralisk_Cavern) &&
            (Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh || supplyUsed >= 60) &&
            allUnitCount[BWAPI::UnitTypes::Zerg_Hydralisk_Den] > 0 &&
            allUnitCount[BWAPI::UnitTypes::Zerg_Greater_Spire] > 0 &&
            allUnitCount[BWAPI::UnitTypes::Zerg_Guardian] > 8);
    }

    // A horrible way of making just enough gatherers gather gas, but it seems to work ok, so don't worry about it for the time being.
    for (auto& u : myUnits)
    {
        if (u->getType() == BWAPI::UnitTypes::Zerg_Extractor && u->isCompleted())
        {
            static int lastAddedGathererToRefinery = 0;
            if (Broodwar->getFrameCount() > lastAddedGathererToRefinery + (3 * 24))
            {
                BWAPI::Unit gasGatherer = u->getClosestUnit(
                    IsOwned && Exists && GetType == BWAPI::UnitTypes::Zerg_Drone &&
                    (CurrentOrder == BWAPI::Orders::MoveToGas || CurrentOrder == BWAPI::Orders::WaitForGas),
                    256);

                if (isSpeedlingBuildOrder &&
                    (/*Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Metabolic_Boost.gasPrice() ||*/
                     Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Metabolic_Boost) /*||
                     Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost) == Broodwar->self()->getMaxUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost)*/))
                {
                    if (gasGatherer != nullptr)
                    {
                        if (!gasGatherer->isConstructing() && noCmdPending(gasGatherer) && gasGatherer->canStop())
                        {
                            gasGatherer->stop();
                            continue;
                        }
                    }
                }
                else
                {
                    if (gasGatherer == nullptr)
                    {        
                        auto isAvailableToGatherFrom =
                            [&u, &noCmdPending](Unit& tmpUnit)
                            {
                                return !tmpUnit->isConstructing() && noCmdPending(tmpUnit) && tmpUnit->canGather(u);
                            };

                        BWAPI::Unit newGasGatherer = newGasGatherer = u->getClosestUnit(GetType == BWAPI::UnitTypes::Zerg_Drone && IsIdle && !IsCarryingSomething && IsOwned && isAvailableToGatherFrom);
                        if (newGasGatherer == nullptr)
                            newGasGatherer = u->getClosestUnit(GetType == BWAPI::UnitTypes::Zerg_Drone && IsGatheringMinerals && !IsCarryingSomething && IsOwned && isAvailableToGatherFrom);
                        if (newGasGatherer == nullptr)
                            newGasGatherer = u->getClosestUnit(GetType == BWAPI::UnitTypes::Zerg_Drone && IsIdle && !IsCarryingGas && IsOwned && isAvailableToGatherFrom);
                        if (newGasGatherer == nullptr)
                            newGasGatherer = u->getClosestUnit(GetType == BWAPI::UnitTypes::Zerg_Drone && IsGatheringMinerals && IsOwned && isAvailableToGatherFrom);
                        // If a unit was found
                        if (newGasGatherer)
                        {
                            newGasGatherer->gather(u);
        
                            if (resourceToGathererMap.find(u) != resourceToGathererMap.end())
                            {
                                gathererToResourceMap.erase(resourceToGathererMap.at(u));
                            }
        
                            resourceToGathererMap[u] = newGasGatherer;
                            gathererToResourceMap[newGasGatherer] = u;
                            lastAddedGathererToRefinery = Broodwar->getFrameCount();
                        }
                    }
        
                    break;
                }
            }
        }
    }

    Unitset myFreeGatherers;

    // The main loop.
    for (auto& u : myUnits)
    {
        if (u->getLastCommandFrame() == Broodwar->getFrameCount() && u->getLastCommand().getType() != BWAPI::UnitCommandTypes::None)
        {
            // Already issued a command to this unit this frame (e.g. a build command) so skip this unit.
            continue;
        }

        if (!u->canCommand() || u->isStuck())
            continue;

        // Cancel morph when appropriate if we are using the extractor trick.
        if (u->getType() == BWAPI::UnitTypes::Zerg_Extractor && !u->isCompleted())
        {
            if (Broodwar->getFrameCount() < transitionOutOfFourPoolFrameCountThresh &&
                supplyUsed < 60 &&
                (completedUnitCount[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 0 ||
                 (supplyUsed >= Broodwar->self()->supplyTotal() - 1 ||
                  ((Broodwar->self()->minerals() < 50 ||
                    allUnitCount[BWAPI::UnitTypes::Zerg_Larva] == 0) &&
                   supplyUsed < Broodwar->self()->supplyTotal() - 3) ||
                  u->getRemainingBuildTime() <= Broodwar->getRemainingLatencyFrames() + 2)))
            {
                if (u->canCancelMorph())
                {
                    u->cancelMorph();
                    continue;
                }
            }
        }

        // Cancel pool if we have no completed/incomplete drones and definitely wouldn't be able
        // to get any drones after it completes (assuming any incomplete hatcheries are left to complete).
        if (u->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool &&
            !u->isCompleted())
        {
            // Note: using allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] rather than incompleteUnitCount[BWAPI::UnitTypes::Zerg_Extractor]
            // because BWAPI seems to think it is completed.
            if (allUnitCount[BWAPI::UnitTypes::Zerg_Drone] + numWorkersTrainedThisFrame + allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] == 0 &&
                (Broodwar->self()->minerals() < 50 ||
                 (allUnitCount[BWAPI::UnitTypes::Zerg_Hatchery] == 0 &&
                  allUnitCount[BWAPI::UnitTypes::Zerg_Lair] == 0 &&
                  allUnitCount[BWAPI::UnitTypes::Zerg_Hive] == 0 &&
                  allUnitCount[BWAPI::UnitTypes::Zerg_Larva] == 0)))
            {
                if (u->canCancelMorph())
                {
                    u->cancelMorph();
                    continue;
                }
            }
        }

        // Cancel egg if it doesn't contain a drone and we have no completed/incomplete drones and definitely wouldn't be able
        // to get any drones if it were to complete (assuming any incomplete hatcheries are left to complete).
        // Note: using allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] rather than incompleteUnitCount[BWAPI::UnitTypes::Zerg_Extractor]
        // because BWAPI seems to think it is completed.
        if (completedUnitCount[BWAPI::UnitTypes::Zerg_Spawning_Pool] > 0 &&
            u->getType() == BWAPI::UnitTypes::Zerg_Egg &&
            !u->isCompleted() &&
            u->getBuildType() != BWAPI::UnitTypes::None &&
            u->getBuildType() != BWAPI::UnitTypes::Unknown &&
            allUnitCount[BWAPI::UnitTypes::Zerg_Drone] + numWorkersTrainedThisFrame + allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] == 0 &&
            (Broodwar->self()->minerals() < 50 ||
             (allUnitCount[BWAPI::UnitTypes::Zerg_Hatchery] == 0 &&
              allUnitCount[BWAPI::UnitTypes::Zerg_Lair] == 0 &&
              allUnitCount[BWAPI::UnitTypes::Zerg_Hive] == 0 &&
              allUnitCount[BWAPI::UnitTypes::Zerg_Larva] == 0)))
        {
            if (u->canCancelMorph())
            {
                u->cancelMorph();
                continue;
            }
        }

        // Cancel pool if a drone has died and we have a low number of workers.
        if (u->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool &&
            !u->isCompleted())
        {
            // Note: using allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] rather than incompleteUnitCount[BWAPI::UnitTypes::Zerg_Extractor]
            // because BWAPI seems to think it is completed.
            if (allUnitCount[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 1 &&
                Broodwar->self()->deadUnitCount(BWAPI::UnitTypes::Zerg_Drone) > 0 &&
                allUnitCount[BWAPI::UnitTypes::Zerg_Drone] + numWorkersTrainedThisFrame + allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] < 3)
            {
                if (u->canCancelMorph())
                {
                    u->cancelMorph();
                    continue;
                }
            }
        }

        // Ignore the unit if it is incomplete or busy constructing
        if (!u->isCompleted() || u->isConstructing())
            continue;

        // For speedling build, upgrade metabolic boost when possible.
        if (isSpeedlingBuildOrder &&
            u->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool)
        {
            if (u->canUpgrade(BWAPI::UpgradeTypes::Metabolic_Boost))
            {
                u->upgrade(BWAPI::UpgradeTypes::Metabolic_Boost);
                continue;
            }
        }

        // Could also take into account higher ground advantage, cover advantage (e.g. in trees), HP regen, shields regen,
        // effects of spells like dark swarm. The list is endless.
        auto getBestEnemyThreatUnitLambda =
            [&u](const BWAPI::Unit& bestSoFarUnit, const BWAPI::Unit& curUnit)
            {
                if (curUnit->isPowered() != bestSoFarUnit->isPowered())
                {
                    return curUnit->isPowered() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isLockedDown() != bestSoFarUnit->isLockedDown())
                {
                    return !curUnit->isLockedDown() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isMaelstrommed() != bestSoFarUnit->isMaelstrommed())
                {
                    return !curUnit->isMaelstrommed() ? curUnit : bestSoFarUnit;
                }

                // Prefer to attack units that can return fire or could be tactical threats in certain scenarios.
                const BWAPI::UnitType curUnitType = curUnit->getType();
                const BWAPI::UnitType bestSoFarUnitType = bestSoFarUnit->getType();
                const BWAPI::WeaponType curUnitWeaponType =
                    u->isFlying() ? curUnitType.airWeapon() : curUnitType.groundWeapon();
                const BWAPI::WeaponType bestSoFarUnitWeaponType =
                    u->isFlying() ? bestSoFarUnitType.airWeapon() : bestSoFarUnitType.groundWeapon();
                if (curUnitWeaponType != bestSoFarUnitWeaponType)
                {
                    if (curUnitWeaponType == BWAPI::WeaponTypes::None &&
                        // FYI, Protoss_Carrier, Hero_Gantrithor, Protoss_Reaver, Hero_Warbringer are the
                        // only BWAPI::UnitType's that have no weapon but UnitType::canAttack() returns true.
                        curUnitType.canAttack() &&
                        curUnitType != BWAPI::UnitTypes::Terran_Bunker &&
                        curUnitType != BWAPI::UnitTypes::Protoss_High_Templar &&
                        curUnitType != BWAPI::UnitTypes::Zerg_Defiler &&
                        curUnitType != BWAPI::UnitTypes::Protoss_Dark_Archon &&
                        curUnitType != BWAPI::UnitTypes::Terran_Science_Vessel &&
                        curUnitType != BWAPI::UnitTypes::Zerg_Queen &&
                        curUnitType != BWAPI::UnitTypes::Protoss_Shuttle &&
                        curUnitType != BWAPI::UnitTypes::Terran_Dropship &&
                        curUnitType != BWAPI::UnitTypes::Protoss_Observer &&
                        curUnitType != BWAPI::UnitTypes::Zerg_Overlord &&
                        curUnitType != BWAPI::UnitTypes::Terran_Medic &&
                        curUnitType != BWAPI::UnitTypes::Terran_Nuclear_Silo &&
                        curUnitType != BWAPI::UnitTypes::Zerg_Nydus_Canal /*&&
                        // TODO: re-enable Terran_Comsat_Station after add any
                        // logic to produce cloaked units.
                        curUnitType != BWAPI::UnitTypes::Terran_Comsat_Station*/)
                    {
                        return bestSoFarUnit;
                    }

                    if (bestSoFarUnitWeaponType == BWAPI::WeaponTypes::None &&
                        // FYI, Protoss_Carrier, Hero_Gantrithor, Protoss_Reaver, Hero_Warbringer are the
                        // only BWAPI::UnitType's that have no weapon but UnitType::canAttack() returns true.
                        bestSoFarUnitType.canAttack() &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Terran_Bunker &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Protoss_High_Templar &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Zerg_Defiler &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Protoss_Dark_Archon &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Terran_Science_Vessel &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Zerg_Queen &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Protoss_Shuttle &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Terran_Dropship &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Protoss_Observer &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Zerg_Overlord &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Terran_Medic &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Terran_Nuclear_Silo &&
                        bestSoFarUnitType != BWAPI::UnitTypes::Zerg_Nydus_Canal /*&&
                        // TODO: re-enable Terran_Comsat_Station after add any
                        // logic to produce cloaked units.
                        bestSoFarUnitType != BWAPI::UnitTypes::Terran_Comsat_Station*/)
                    {
                        return curUnit;
                    }
                }

                auto unitTypeScoreLambda = [](const BWAPI::UnitType& unitType) -> int
                    {
                        return
                            unitType == BWAPI::UnitTypes::Protoss_Pylon ? 30000 :
                            unitType == BWAPI::UnitTypes::Protoss_Nexus ? 29000 :
                            unitType == BWAPI::UnitTypes::Terran_Command_Center ? 28000 :
                            unitType == BWAPI::UnitTypes::Zerg_Hive ? 27000 :
                            unitType == BWAPI::UnitTypes::Zerg_Lair ? 26000 :
                            unitType == BWAPI::UnitTypes::Zerg_Hatchery ? 25000 :
                            unitType == BWAPI::UnitTypes::Zerg_Greater_Spire ? 24000 :
                            unitType == BWAPI::UnitTypes::Zerg_Spire ? 23000 :
                            unitType == BWAPI::UnitTypes::Terran_Starport ? 22000 :
                            unitType == BWAPI::UnitTypes::Protoss_Stargate ? 21000 :
                            unitType == BWAPI::UnitTypes::Terran_Factory ? 20000 :
                            unitType == BWAPI::UnitTypes::Terran_Barracks ? 19000 :
                            unitType == BWAPI::UnitTypes::Zerg_Spawning_Pool ? 18000 :
                            unitType == BWAPI::UnitTypes::Zerg_Hydralisk_Den ? 17000 :
                            unitType == BWAPI::UnitTypes::Zerg_Queens_Nest ? 16000 :
                            unitType == BWAPI::UnitTypes::Protoss_Templar_Archives ? 15000 :
                            unitType == BWAPI::UnitTypes::Protoss_Gateway ? 14000 :
                            unitType == BWAPI::UnitTypes::Protoss_Cybernetics_Core ? 13000 :
                            unitType == BWAPI::UnitTypes::Protoss_Shield_Battery ? 12000 :
                            unitType == BWAPI::UnitTypes::Protoss_Forge ? 11000 :
                            unitType == BWAPI::UnitTypes::Protoss_Citadel_of_Adun ? 10000 :
                            unitType == BWAPI::UnitTypes::Terran_Academy ? 9000 :
                            unitType == BWAPI::UnitTypes::Terran_Engineering_Bay ? 8000 :
                            unitType == BWAPI::UnitTypes::Zerg_Creep_Colony ? 7000 :
                            unitType == BWAPI::UnitTypes::Zerg_Evolution_Chamber ? 6000 :
                            unitType == BWAPI::UnitTypes::Zerg_Lurker_Egg ? 5000 :
                            unitType == BWAPI::UnitTypes::Zerg_Egg ? 4000 :
                            unitType == BWAPI::UnitTypes::Zerg_Larva ? 3000 :
                            unitType == BWAPI::UnitTypes::Zerg_Spore_Colony ? 2000 :
                            unitType == BWAPI::UnitTypes::Terran_Missile_Turret ? 1000 :
                            unitType == BWAPI::UnitTypes::Terran_Supply_Depot ? -1000 :
                            unitType.isRefinery() ? -2000 :
                            unitType == BWAPI::UnitTypes::Terran_Covert_Ops ? -3000 :
                            unitType == BWAPI::UnitTypes::Terran_Control_Tower ? -4000 :
                            unitType == BWAPI::UnitTypes::Terran_Machine_Shop ? -5000 :
                            unitType == BWAPI::UnitTypes::Terran_Comsat_Station ? -6000 :
                            unitType == BWAPI::UnitTypes::Protoss_Scarab ? -7000 :
                            unitType == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine ? -8000 :
                            unitType == BWAPI::UnitTypes::Zerg_Infested_Terran ? -9000 :
                            0;
                    };

                const int curUnitTypeScore = unitTypeScoreLambda(curUnitType);
                const int bestSoFarUnitTypeScore = unitTypeScoreLambda(bestSoFarUnitType);
                if (curUnitTypeScore != bestSoFarUnitTypeScore)
                {
                    return curUnitTypeScore > bestSoFarUnitTypeScore ? curUnit : bestSoFarUnit;
                }

                // If the set of units being considered only contains workers or contains no workers
                // then this should work as intended.
                if (curUnit->getType().isWorker() && bestSoFarUnit->getType().isWorker() &&
                    !u->isInWeaponRange(curUnit) && !u->isInWeaponRange(bestSoFarUnit) &&
                    u->getDistance(curUnit) != u->getDistance(bestSoFarUnit))
                {
                    return (u->getDistance(curUnit) < u->getDistance(bestSoFarUnit)) ? curUnit : bestSoFarUnit;
                }

                const int curUnitLifeForceScore =
                    curUnit->getHitPoints() + curUnit->getShields() + curUnitType.armor() + curUnit->getDefenseMatrixPoints();
                const int bestSoFarUnitLifeForceScore =
                    bestSoFarUnit->getHitPoints() + bestSoFarUnit->getShields() + bestSoFarUnitType.armor() + bestSoFarUnit->getDefenseMatrixPoints();
                if (curUnitLifeForceScore != bestSoFarUnitLifeForceScore)
                {
                    return curUnitLifeForceScore < bestSoFarUnitLifeForceScore ? curUnit : bestSoFarUnit;
                }

                // Whether irradiate is good or bad is very situational (it depends whether it is
                // positioned amongst more of my units than the enemy's) but for now let's assume
                // it is positioned amongst more of mine. TODO: add special logic once my bot can
                // cast irradiate.
                if (curUnit->isIrradiated() != bestSoFarUnit->isIrradiated())
                {
                    return !curUnit->isIrradiated() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isBeingHealed() != bestSoFarUnit->isBeingHealed())
                {
                    return !curUnit->isBeingHealed() ? curUnit : bestSoFarUnit;
                }

                if (curUnitType.regeneratesHP() != bestSoFarUnitType.regeneratesHP())
                {
                    return !curUnitType.regeneratesHP() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isRepairing() != bestSoFarUnit->isRepairing())
                {
                    return curUnit->isRepairing() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isConstructing() != bestSoFarUnit->isConstructing())
                {
                    return curUnit->isConstructing() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isPlagued() != bestSoFarUnit->isPlagued())
                {
                    return !curUnit->isPlagued() ? curUnit : bestSoFarUnit;
                }

                if ((curUnit->getTarget() == u) != (bestSoFarUnit->getTarget() == u) || (curUnit->getOrderTarget() == u) != (bestSoFarUnit->getOrderTarget() == u))
                {
                    return ((curUnit->getTarget() == u && bestSoFarUnit->getTarget() != u) || (curUnit->getOrderTarget() == u && bestSoFarUnit->getOrderTarget() != u)) ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isAttacking() != bestSoFarUnit->isAttacking())
                {
                    return curUnit->isAttacking() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->getSpellCooldown() != bestSoFarUnit->getSpellCooldown())
                {
                    return curUnit->getSpellCooldown() < bestSoFarUnit->getSpellCooldown() ? curUnit : bestSoFarUnit;
                }

                if (!u->isFlying())
                {
                    if (curUnit->getGroundWeaponCooldown() != bestSoFarUnit->getGroundWeaponCooldown())
                    {
                        return curUnit->getGroundWeaponCooldown() < bestSoFarUnit->getGroundWeaponCooldown() ? curUnit : bestSoFarUnit;
                    }
                }
                else
                {    
                    if (curUnit->getAirWeaponCooldown() != bestSoFarUnit->getAirWeaponCooldown())
                    {
                        return curUnit->getAirWeaponCooldown() < bestSoFarUnit->getAirWeaponCooldown() ? curUnit : bestSoFarUnit;
                    }
                }

                if (curUnit->isStartingAttack() != bestSoFarUnit->isStartingAttack())
                {
                    return !curUnit->isStartingAttack() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isAttackFrame() != bestSoFarUnit->isAttackFrame())
                {
                    return !curUnit->isAttackFrame() ? curUnit : bestSoFarUnit;
                }

                // Prefer stationary targets (because more likely to hit them).
                if (curUnit->isHoldingPosition() != bestSoFarUnit->isHoldingPosition())
                {
                    return curUnit->isHoldingPosition() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isMoving() != bestSoFarUnit->isMoving())
                {
                    return !curUnit->isMoving() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isBraking() != bestSoFarUnit->isBraking())
                {
                    if (curUnit->isMoving() && bestSoFarUnit->isMoving())
                    {
                        return curUnit->isBraking() ? curUnit : bestSoFarUnit;
                    }
                    else if (!curUnit->isMoving() && !bestSoFarUnit->isMoving())
                    {
                        return !curUnit->isBraking() ? curUnit : bestSoFarUnit;
                    }
                }

                if (curUnit->isAccelerating() != bestSoFarUnit->isAccelerating())
                {
                    if (curUnit->isMoving() && bestSoFarUnit->isMoving())
                    {
                        return !curUnit->isAccelerating() ? curUnit : bestSoFarUnit;
                    }
                    else if (!curUnit->isMoving() && !bestSoFarUnit->isMoving())
                    {
                        return !curUnit->isAccelerating() ? curUnit : bestSoFarUnit;
                    }
                }

                // Prefer to attack enemy units that are morphing. Assume here that armor has already taken into account properly above.
                if (curUnit->isMorphing() != bestSoFarUnit->isMorphing())
                {
                    return curUnit->isMorphing() ? curUnit : bestSoFarUnit;
                }

                // Prefer to attack enemy units that are being constructed.
                if (curUnit->isBeingConstructed() != bestSoFarUnit->isBeingConstructed())
                {
                    return curUnit->isBeingConstructed() ? curUnit : bestSoFarUnit;
                }

                // Prefer to attack enemy units that are incomplete.
                if (curUnit->isCompleted() != bestSoFarUnit->isCompleted())
                {
                    return !curUnit->isCompleted() ? curUnit : bestSoFarUnit;
                }

                // Prefer to attack bunkers.
                // Note: getType()->canAttack() is false for a bunker.
                if ((curUnitType == BWAPI::UnitTypes::Terran_Bunker || bestSoFarUnitType == BWAPI::UnitTypes::Terran_Bunker) &&
                    curUnitType != bestSoFarUnitType)
                {
                    return curUnitType == BWAPI::UnitTypes::Terran_Bunker ? curUnit : bestSoFarUnit;
                }

                // Prefer to attack enemy units that can attack.
                if (curUnitType.canAttack() != bestSoFarUnitType.canAttack())
                {
                    return curUnitType.canAttack() ? curUnit : bestSoFarUnit;
                }

                // Prefer to attack workers.
                if (curUnitType.isWorker() != bestSoFarUnitType.isWorker())
                {
                    return curUnitType.isWorker() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isCarryingGas() != bestSoFarUnit->isCarryingGas())
                {
                    return curUnit->isCarryingGas() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isCarryingMinerals() != bestSoFarUnit->isCarryingMinerals())
                {
                    return curUnit->isCarryingMinerals() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isGatheringMinerals() != bestSoFarUnit->isGatheringMinerals())
                {
                    return curUnit->isGatheringMinerals() ? curUnit : bestSoFarUnit;
                }

                // For now, let's prefer to attack mineral gatherers than gas gatherers,
                // because gas gatherers generally take longer to kill because they keep
                // going into the refinery/assimilator/extractor.
                if (curUnit->isGatheringGas() != bestSoFarUnit->isGatheringGas())
                {
                    return curUnit->isGatheringGas() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->getPowerUp() != bestSoFarUnit->getPowerUp())
                {
                    if (bestSoFarUnit->getPowerUp() == nullptr)
                    {
                        return curUnit;
                    }
                    else if (curUnit->getPowerUp() == nullptr)
                    {
                        return bestSoFarUnit;
                    }
                }

                if (curUnit->isBlind() != bestSoFarUnit->isBlind())
                {
                    return !curUnit->isBlind() ? curUnit : bestSoFarUnit;
                }

                if ((curUnitType == BWAPI::UnitTypes::Protoss_Carrier || curUnitType == UnitTypes::Hero_Gantrithor) &&
                    (bestSoFarUnitType == BWAPI::UnitTypes::Protoss_Carrier || bestSoFarUnitType == UnitTypes::Hero_Gantrithor) &&
                    curUnit->getInterceptorCount() != bestSoFarUnit->getInterceptorCount())
                {
                    return curUnit->getInterceptorCount() > bestSoFarUnit->getInterceptorCount() ? curUnit : bestSoFarUnit;
                }

                if (u->getDistance(curUnit) != u->getDistance(bestSoFarUnit))
                {
                    return (u->getDistance(curUnit) < u->getDistance(bestSoFarUnit)) ? curUnit : bestSoFarUnit;
                }

                if (curUnit->getAcidSporeCount() != bestSoFarUnit->getAcidSporeCount())
                {
                    return curUnit->getAcidSporeCount() < bestSoFarUnit->getAcidSporeCount() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->getKillCount() != bestSoFarUnit->getKillCount())
                {
                    return curUnit->getKillCount() < bestSoFarUnit->getKillCount() ? curUnit : bestSoFarUnit;
                }

                if (curUnit->isIdle() != bestSoFarUnit->isIdle())
                {
                    return !curUnit->isIdle() ? curUnit : bestSoFarUnit;
                }

                // TODO: The meaning of isUnderAttack() is more like  "was attacked recently" and from the forums it sounds
                // like it is a GUI thing and affected by the real clock (not the in-game clock) so if games are played at
                // high speed it is misleading, but let's check it anyway as lowest priority until I can come up with more
                // reliable logic. Could also check whether any of our other units are targeting it (if that info is
                // accessible).
                if (curUnit->isUnderAttack() != bestSoFarUnit->isUnderAttack())
                {
                    return curUnit->isUnderAttack() ? curUnit : bestSoFarUnit;
                }

                return bestSoFarUnit;
            };

        const BWAPI::UnitType workerUnitType = Broodwar->self()->getRace().getWorker();

        if (u->getType().isWorker())
        {
            // Attack enemy units of opportunity (e.g. enemy worker scout(s) that are harassing my gatherers).
            // Don't issue a new command if we are a low life drone planning to build an extractor.
            if ((u != lowLifeDrone || u != extractorBuilder) &&
                (u->isIdle() || u->getLastCommand().getType() == BWAPI::UnitCommandTypes::None ||
                 Broodwar->getFrameCount() >= u->getLastCommandFrame() + 2 - (Broodwar->getLatencyFrames() > 2 ? u->getLastCommandFrame() % 2 : 0)))
            {
                // Add some frames to cover frame(s) that might be needed to change direction.
                if (u->getGroundWeaponCooldown() <= Broodwar->getRemainingLatencyFrames() + 2)
                {
                    const BWAPI::Unit bestAttackableEnemyNonBuildingUnit =
                        workerAttackTargetUnit != nullptr ?
                        workerAttackTargetUnit :
                        Broodwar->getBestUnit(
                            getBestEnemyThreatUnitLambda,
                            IsEnemy && IsVisible && IsDetected && Exists &&
                            // Ignore buildings because we do not want to waste mining time, and I don't think we need
                            // to worry about manner pylon or gas steal because the current 4pool-only version in theory shouldn't
                            // place workers where they can get stuck by a manner pylon on most maps, and gas steal is
                            // rarely much of a hinderance except on large maps because we need lots of lings to be in a
                            // situation to use the extractor trick (it just stops us healing drones with the extractor trick).
                            // The lings will attack buildings near my base when they spawn anyway.
                            !IsBuilding &&
                            [&u](Unit& tmpUnit) { return u->isInWeaponRange(tmpUnit) && u->canAttack(PositionOrUnit(tmpUnit)); },
                            u->getPosition(),
                            std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()));
    
                    if (bestAttackableEnemyNonBuildingUnit && u->canAttack(PositionOrUnit(bestAttackableEnemyNonBuildingUnit)))
                    {
                        const BWAPI::Unit oldOrderTarget = u->getTarget();
                        if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != bestAttackableEnemyNonBuildingUnit)
                        {
                            u->attack(bestAttackableEnemyNonBuildingUnit);
    
                            if (gathererToResourceMap.find(u) != gathererToResourceMap.end() && resourceToGathererMap.find(gathererToResourceMap.at(u)) != resourceToGathererMap.end() && resourceToGathererMap.at(gathererToResourceMap.at(u)) == u)
                            {
                                resourceToGathererMap.erase(gathererToResourceMap.at(u));
                            }
    
                            gathererToResourceMap.erase(u);
                        }

                        continue;
                    }
                }

                // Don't issue a return cargo or gather command if we are a drone planning to build a building.
                if (u != extractorBuilder && u != groundArmyBuildingBuilder && u->getLastCommand().getType() != BWAPI::UnitCommandTypes::Build)
                {
                    // If idle or were targeting an enemy unit or are no longer carrying minerals...
                    const bool isNewCmdNeeded = u->isIdle() || (u->getTarget() && u->getTarget()->getPlayer() && u->getTarget()->getPlayer()->isEnemy(Broodwar->self()));
                    if (isNewCmdNeeded ||
                        (!u->isCarryingMinerals() && (int) u->getClientInfo(wasJustCarryingMineralsInd) == wasJustCarryingMineralsTrueVal))
                    {
                        if (!u->isCarryingMinerals() && (int) u->getClientInfo(wasJustCarryingMineralsInd) == wasJustCarryingMineralsTrueVal)
                        {
                            // Reset indicator about carrying minerals because we aren't carrying minerals now.
                            // Note: setClientInfo may also be called at the end of this and some other frames
                            // (but not necessarily at the end of all frames because there's logic at the start
                            // of each frame to return if the frame count modulo is a certain value).
                            u->setClientInfo(wasJustCarryingMineralsDefaultVal, wasJustCarryingMineralsInd);
                        }
    
                        // Order workers carrying a resource to return them to the center,
                        // otherwise find a mineral patch to harvest.
                        if (isNewCmdNeeded &&
                            (u->isCarryingGas() || u->isCarryingMinerals()))
                        {
                            if (u->canReturnCargo())
                            {
                                if (u->getLastCommand().getType() != BWAPI::UnitCommandTypes::Return_Cargo)
                                {
                                    u->returnCargo();
                                }
    
                                continue;
                            }
                        }
                        // The worker cannot harvest anything if it is carrying a powerup such as a flag.
                        else if (!u->getPowerUp())
                        {
                            if (u->canGather())
                            {
                                myFreeGatherers.insert(u);
                            }
                        }
                    }
                }
            }

            continue;
        }
        else if (u->getType() == workerUnitType.whatBuilds().first)
        {
            // Train more workers if we have less than 3 (or 6 against enemy worker rush until pool is building), or less than 28 late-game.
            // Note: one of the workers could currently be doing the extractor trick.
            // Note: using allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] rather than incompleteUnitCount[BWAPI::UnitTypes::Zerg_Extractor]
            // because BWAPI seems to think it is completed.
            if ((allUnitCount[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 0 &&
                 // If we have lost drones to enemy worker rush then keep adding a few extra drones until they start to pop.
                 ((Broodwar->self()->deadUnitCount(BWAPI::UnitTypes::Zerg_Drone) > 0 &&
                   allUnitCount[BWAPI::UnitTypes::Zerg_Drone] + numWorkersTrainedThisFrame + allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] < 6 &&
                   myCompletedWorkers.size() + allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] < 4) ||
                  allUnitCount[BWAPI::UnitTypes::Zerg_Drone] + numWorkersTrainedThisFrame + allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] < 4)) ||
                (Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Zerg_Spawning_Pool) > 0 &&
                 allUnitCount[BWAPI::UnitTypes::Zerg_Drone] + numWorkersTrainedThisFrame + allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] < 3) ||
                ((Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh || supplyUsed >= 60) &&
                 // TODO: this might not count the worker currently inside the extractor.
                 allUnitCount[BWAPI::UnitTypes::Zerg_Drone] + numWorkersTrainedThisFrame < ((isSpeedlingBuildOrder && allUnitCount[BWAPI::UnitTypes::Zerg_Lair] + allUnitCount[BWAPI::UnitTypes::Zerg_Hive] == 0) ? 9 : 28)))
            {
                if (u->getType() == UnitTypes::Zerg_Larva ||
                    (u->getTrainingQueue().size() < 2 &&
                     noCmdPending(u)))
                {
                    if (u->canTrain(workerUnitType))
                    {
                        u->train(workerUnitType);
                        ++numWorkersTrainedThisFrame;
                        continue;
                    }
                }
            }
        }
        else if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery)
        {
            // Morph to lair late-game.
            static bool issuedMorphLairCmd = false;
            if (u->canMorph(BWAPI::UnitTypes::Zerg_Lair) &&
                allUnitCount[BWAPI::UnitTypes::Zerg_Lair] + allUnitCount[BWAPI::UnitTypes::Zerg_Hive] == 0 &&
                (!isSpeedlingBuildOrder ?
                 true :
                 (Broodwar->getFrameCount() > (5 * 60 * 24) &&
                  (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost) == Broodwar->self()->getMaxUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost) ||
                   Broodwar->self()->isUpgrading(BWAPI::UpgradeTypes::Metabolic_Boost)))))
            {
                u->morph(BWAPI::UnitTypes::Zerg_Lair);
                issuedMorphLairCmd = true;
                continue;
            }
        }
        else if (u->getType() == BWAPI::UnitTypes::Zerg_Lair)
        {
            // Morph to hive late-game.
            static bool issuedMorphHiveCmd = false;
            if (u->canMorph(BWAPI::UnitTypes::Zerg_Hive) && allUnitCount[BWAPI::UnitTypes::Zerg_Hive] == 0)
            {
                u->morph(BWAPI::UnitTypes::Zerg_Hive);
                issuedMorphHiveCmd = true;
                continue;
            }
        }
        else if (u->getType() == BWAPI::UnitTypes::Zerg_Spire)
        {
            // Morph to greater spire late-game.
            static bool issuedMorphGreaterSpireCmd = false;
            if (u->canMorph(BWAPI::UnitTypes::Zerg_Greater_Spire) && allUnitCount[BWAPI::UnitTypes::Zerg_Greater_Spire] == 0)
            {
                u->morph(BWAPI::UnitTypes::Zerg_Greater_Spire);
                issuedMorphGreaterSpireCmd = true;
                continue;
            }
        }
        else if (u->getType() == BWAPI::UnitTypes::Zerg_Mutalisk)
        {
            // Morph a limited number of guardians late-game.
            // Not until at least one mutalisk has died though (because mutalisks may be all we need,
            // e.g. against lifted buildings or an enemy that doesn't get any anti-air).
            if (u->canMorph(BWAPI::UnitTypes::Zerg_Guardian) && allUnitCount[UnitTypes::Zerg_Guardian] <= 8 &&
                Broodwar->self()->deadUnitCount(UnitTypes::Zerg_Mutalisk) > 0)
            {
                u->morph(BWAPI::UnitTypes::Zerg_Guardian);
                continue;
            }
        }

        const BWAPI::UnitType airForceUnitType = BWAPI::UnitTypes::Zerg_Mutalisk;

        if (u->getType() == airForceUnitType.whatBuilds().first)
        {
            // Don't make mutalisks until we have a greater spire (or a greater spire morphing),
            // to ensure we have enough gas to tech up to & morph greater spire (rather than
            // constantly kamikaze'ing mutalisks).
            if ((isSpeedlingBuildOrder ||
                 allUnitCount[BWAPI::UnitTypes::Zerg_Greater_Spire] > 0) &&
                allUnitCount[airForceUnitType] <= (isSpeedlingBuildOrder ? 40 : 12))
            {
                // Train more air combat units.
                if (u->getType() == UnitTypes::Zerg_Larva || (u->getTrainingQueue().size() < 2 && noCmdPending(u)))
                {
                    if (u->canTrain(airForceUnitType))
                    {
                        u->train(airForceUnitType);
                        continue;
                    }
                }
            }
        }

        if (u->getType() == groundArmyUnitType.whatBuilds().first)
        {
            // Train more ground combat units.
            if ((Broodwar->getFrameCount() < transitionOutOfFourPoolFrameCountThresh ||
                 ((completedUnitCount[UnitTypes::Zerg_Ultralisk_Cavern] == 0 || allUnitCount[UnitTypes::Zerg_Ultralisk] >= 2) &&
                  allUnitCount[groundArmyUnitType] <= 30)) &&
                (u->getType() == UnitTypes::Zerg_Larva ||
                 (u->getTrainingQueue().size() < 2 &&
                  noCmdPending(u))))
            {
                if (u->canTrain(groundArmyUnitType))
                {
                    u->train(groundArmyUnitType);
                    continue;
                }
            }
        }

        if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk.whatBuilds().first)
        {
            // Train more hydralisk units.
            if (allUnitCount[UnitTypes::Zerg_Guardian] >= 8 &&
                allUnitCount[BWAPI::UnitTypes::Zerg_Hydralisk] <= 20)
            {
                if (u->canTrain(BWAPI::UnitTypes::Zerg_Hydralisk))
                {
                    u->train(BWAPI::UnitTypes::Zerg_Hydralisk);
                    continue;
                }
            }
        }

        if (u->getType() == BWAPI::UnitTypes::Zerg_Ultralisk.whatBuilds().first)
        {
            // Train more ultralisk units.
            if (allUnitCount[UnitTypes::Zerg_Guardian] >= 8 &&
                allUnitCount[BWAPI::UnitTypes::Zerg_Ultralisk] <= 6)
            {
                if (u->canTrain(BWAPI::UnitTypes::Zerg_Ultralisk))
                {
                    u->train(BWAPI::UnitTypes::Zerg_Ultralisk);
                    continue;
                }
            }
        }

        // A resource depot is a Command Center, Nexus, or Hatchery/Lair/Hive.
        if (u->getType().isResourceDepot())
        {
            const UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
            // Commented out because for 4pool we probably shouldn't ever make overlords
            // cos we want all available larvae available to replenish ling count if ling(s) die.
            // We could use the amount of larvae available as a threshold but there is a risk many
            // lings could soon die and we wouldn't have any larvae available to re-make them.
            //if (incompleteUnitCount[supplyProviderType] == 0 &&
            //    ((Broodwar->getFrameCount() < transitionOutOfFourPoolFrameCountThresh && supplyUsed >= Broodwar->self()->supplyTotal()) ||
            //     (Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh && Broodwar->self()->supplyTotal() < 400)))
            if ((Broodwar->getFrameCount() < transitionOutOfFourPoolFrameCountThresh &&
                 Broodwar->self()->supplyTotal() + (incompleteUnitCount[supplyProviderType] * supplyProviderType.supplyProvided()) < 18) ||
                (Broodwar->getFrameCount() >= transitionOutOfFourPoolFrameCountThresh &&
                 Broodwar->self()->supplyTotal() + (incompleteUnitCount[supplyProviderType] * supplyProviderType.supplyProvided()) < 400 &&
                 supplyUsed + 16 > Broodwar->self()->supplyTotal() + (incompleteUnitCount[supplyProviderType] * supplyProviderType.supplyProvided()) &&
                 (supplyUsed > 18 || supplyUsed + 2 > Broodwar->self()->supplyTotal() + (incompleteUnitCount[supplyProviderType] * supplyProviderType.supplyProvided()))))
            {
                static int lastIssuedBuildSupplyProviderCmd = 0;
                if (Broodwar->getFrameCount() >= lastIssuedBuildSupplyProviderCmd + (10 * 24))
                {
                    // Retrieve a unit that is capable of constructing the supply needed
                    Unit supplyBuilder = u->getClosestUnit(GetType == supplyProviderType.whatBuilds().first && (IsIdle || IsGatheringMinerals) && IsOwned);
                    // If a unit was found
                    if (supplyBuilder)
                    {
                        if (supplyProviderType.isBuilding())
                        {
                            const TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
                            if (targetBuildLocation && supplyBuilder->canBuild(supplyProviderType, targetBuildLocation))
                            {
                                // Order the builder to construct the supply structure
                                supplyBuilder->build(supplyProviderType, targetBuildLocation);
                                lastIssuedBuildSupplyProviderCmd = Broodwar->getFrameCount();
                                continue;
                            }
                        }
                        else if (supplyBuilder->canTrain(supplyProviderType))
                        {
                            // Train the supply provider (Zerg_Overlord) if the provider is not a structure
                            supplyBuilder->train(supplyProviderType);
                            lastIssuedBuildSupplyProviderCmd = Broodwar->getFrameCount();
                            continue;
                        }
                    }
                }
            }
        }
        // Attempt to detect and fix bugged (frozen) ground units caused by bug in Broodwar.
        // TODO: improve this to speed up detection/fix.
        else if (u->canStop() &&
                 u->canAttack() &&
                 u->canMove() &&
                 !u->isFlying() &&
                 !u->isAttacking() &&
                 (int) u->getClientInfo(frameLastStoppedInd) + (3 * 24) < Broodwar->getFrameCount() &&
                 (int) u->getClientInfo(frameLastAttackingInd) + std::max(Broodwar->self()->weaponDamageCooldown(u->getType()), u->getType().airWeapon().damageCooldown()) + (3 * 24) < Broodwar->getFrameCount() &&
                 (int) u->getClientInfo(frameLastChangedPosInd) > 0 && (int) u->getClientInfo(frameLastChangedPosInd) + (3 * 24) < Broodwar->getFrameCount() &&
                 noCmdPending(u))
        {
            u->stop();
            u->setClientInfo(Broodwar->getFrameCount(), frameLastStoppedInd);
            continue;
        }
        else if (u->canAttack() &&
                 !u->isAttackFrame() &&
                 noCmdPending(u))
        {
            // I.E. in-range enemy unit that is a threat to this particular unit
            // (so for example, an enemy zergling is not a threat to my mutalisk).
            const BWAPI::Unit bestAttackableInRangeEnemySelfThreatUnit =
                // Could also take into account higher ground advantage, cover advantage (e.g. in trees), HP regen, shields regen,
                // effects of spells like dark swarm. The list is endless.
                Broodwar->getBestUnit(
                    getBestEnemyThreatUnitLambda,
                    IsEnemy && IsVisible && IsDetected && Exists &&
                    !IsWorker &&
                    // Warning: some calls like tmpUnit->canAttack(PositionOrUnit(tmpUnit2)) and tmpUnit2->isVisible(tmpUnit->getPlayer())
                    // will always return false because tmpUnit is not commandable by Broodwar->self() and BWAPI doesn't seem to update
                    // unit visibility info correctly for other players than Broodwar->self().
                    // I check !IsLockedDown etc becuase rather than attacking them we would rather fall through and attack workers if possible.
                    !IsLockedDown && !IsMaelstrommed && !IsStasised &&
                    (CanAttack ||
                     GetType == BWAPI::UnitTypes::Terran_Bunker ||
                     GetType == BWAPI::UnitTypes::Protoss_High_Templar ||
                     GetType == BWAPI::UnitTypes::Zerg_Defiler ||
                     GetType == BWAPI::UnitTypes::Protoss_Dark_Archon ||
                     GetType == BWAPI::UnitTypes::Terran_Science_Vessel ||
                     GetType == BWAPI::UnitTypes::Zerg_Queen ||
                     GetType == BWAPI::UnitTypes::Protoss_Shuttle ||
                     GetType == BWAPI::UnitTypes::Terran_Dropship ||
                     GetType == BWAPI::UnitTypes::Protoss_Observer ||
                     GetType == BWAPI::UnitTypes::Zerg_Overlord ||
                     GetType == BWAPI::UnitTypes::Terran_Medic ||
                     GetType == BWAPI::UnitTypes::Terran_Nuclear_Silo ||
                     GetType == BWAPI::UnitTypes::Zerg_Nydus_Canal /*||
                     // TODO: re-enable Terran_Comsat_Station after add any
                     // logic to produce cloaked units.
                     GetType == BWAPI::UnitTypes::Terran_Comsat_Station*/) &&
                    [&u](Unit& tmpUnit)
                    {
                        return
                            u->canAttack(PositionOrUnit(tmpUnit)) &&
                            u->isInWeaponRange(tmpUnit) &&
                            // TODO: add special logic for zerglings.
                            (tmpUnit->getType() != BWAPI::UnitTypes::Terran_Bunker ||
                             u->getType() != BWAPI::UnitTypes::Zerg_Zergling) &&
                            (!tmpUnit->getType().canAttack() ||
                             (!u->isFlying() ? tmpUnit->getType().groundWeapon() : tmpUnit->getType().airWeapon()) != BWAPI::WeaponTypes::None);
                    },
                    u->getPosition(),
                    std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + std::max(Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()), Broodwar->self()->weaponMaxRange(u->getType().airWeapon())));

            if (bestAttackableInRangeEnemySelfThreatUnit)
            {
                const BWAPI::Unit oldOrderTarget = u->getTarget();
                if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != bestAttackableInRangeEnemySelfThreatUnit)
                {
                    u->attack(bestAttackableInRangeEnemySelfThreatUnit);
                }
                continue;
            }

            // I.E. a nearby enemy unit that is a threat to this particular unit
            // (so for example, an enemy zergling is not a threat to my mutalisk).
            const BWAPI::Unit bestAttackableEnemySelfThreatUnit =
                // Could also take into account higher ground advantage, cover advantage (e.g. in trees), HP regen, shields regen,
                // effects of spells like dark swarm. The list is endless.
                Broodwar->getBestUnit(
                    getBestEnemyThreatUnitLambda,
                    IsEnemy && IsVisible && IsDetected && Exists &&
                    !IsWorker &&
                    // Warning: some calls like tmpUnit->canAttack(PositionOrUnit(tmpUnit2)) and tmpUnit2->isVisible(tmpUnit->getPlayer())
                    // will always return false because tmpUnit is not commandable by Broodwar->self() and BWAPI doesn't seem to update
                    // unit visibility info correctly for other players than Broodwar->self().
                    // I check !IsLockedDown etc becuase rather than attacking them we would rather fall through and attack workers if possible.
                    !IsLockedDown && !IsMaelstrommed && !IsStasised &&
                    (CanAttack ||
                     GetType == BWAPI::UnitTypes::Terran_Bunker ||
                     GetType == BWAPI::UnitTypes::Protoss_High_Templar ||
                     GetType == BWAPI::UnitTypes::Zerg_Defiler ||
                     GetType == BWAPI::UnitTypes::Protoss_Dark_Archon ||
                     GetType == BWAPI::UnitTypes::Terran_Science_Vessel ||
                     GetType == BWAPI::UnitTypes::Zerg_Queen ||
                     GetType == BWAPI::UnitTypes::Protoss_Shuttle ||
                     GetType == BWAPI::UnitTypes::Terran_Dropship ||
                     GetType == BWAPI::UnitTypes::Protoss_Observer ||
                     GetType == BWAPI::UnitTypes::Zerg_Overlord ||
                     GetType == BWAPI::UnitTypes::Terran_Medic ||
                     GetType == BWAPI::UnitTypes::Terran_Nuclear_Silo ||
                     GetType == BWAPI::UnitTypes::Zerg_Nydus_Canal /*||
                     // TODO: re-enable Terran_Comsat_Station after add any
                     // logic to produce cloaked units.
                     GetType == BWAPI::UnitTypes::Terran_Comsat_Station*/) &&
                    [&u](Unit& tmpUnit)
                    {
                        return
                            u->canAttack(PositionOrUnit(tmpUnit)) &&
                            // TODO: add special logic for zerglings.
                            (tmpUnit->getType() != BWAPI::UnitTypes::Terran_Bunker ||
                             (u->getType() != BWAPI::UnitTypes::Zerg_Zergling &&
                              // Ignore ghosts long range for now - assume there are marine(s) in the bunker.
                              // Only attack if we are in the bunker's range or we can out-range the bunker.
                              (tmpUnit->getDistance(u) <= tmpUnit->getPlayer()->weaponMaxRange(BWAPI::UnitTypes::Terran_Marine.groundWeapon()) ||
                               u->getPlayer()->weaponMaxRange(u->getType().groundWeapon()) > tmpUnit->getPlayer()->weaponMaxRange(BWAPI::UnitTypes::Terran_Marine.groundWeapon())))) &&
                            ((!tmpUnit->getType().canAttack() ||
                              (!u->isFlying() ? tmpUnit->getType().groundWeapon() : tmpUnit->getType().airWeapon()) != BWAPI::WeaponTypes::None) &&
                             tmpUnit->getDistance(u) <= (int) (std::max(std::max(tmpUnit->getPlayer()->weaponMaxRange(!u->isFlying() ? tmpUnit->getType().groundWeapon() : tmpUnit->getType().airWeapon()),
                                                                                 u->getPlayer()->weaponMaxRange(!tmpUnit->isFlying() ? u->getType().groundWeapon() : u->getType().airWeapon())),
                                                                        112)
                                                               + 32) &&
                             tmpUnit->getClosestUnit(
                                 Exists && GetPlayer == Broodwar->self(),
                                 (int) (std::max(std::max((!u->isFlying() ? tmpUnit->getPlayer()->weaponMaxRange(tmpUnit->getType().groundWeapon()) : tmpUnit->getPlayer()->weaponMaxRange(tmpUnit->getType().airWeapon())),
                                                          (!tmpUnit->isFlying() ? u->getPlayer()->weaponMaxRange(u->getType().groundWeapon()) : u->getPlayer()->weaponMaxRange(u->getType().airWeapon()))),
                                                 112))) != nullptr);
                    },
                    u->getPosition(),
                    // Note: 384 is the max range of any weapon (i.e. siege tank's weapon).
                    // FYI, the max sight range of any unit is 352, and the max seek range of any unit is 288.
                    std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + 384 + 112 + 32);

            if (bestAttackableEnemySelfThreatUnit)
            {
                const BWAPI::Unit oldOrderTarget = u->getTarget();
                if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != bestAttackableEnemySelfThreatUnit)
                {
                    u->attack(bestAttackableEnemySelfThreatUnit);
                }
                continue;
            }

            // Attack enemy worker targets of opportunity.
            const BWAPI::Unit bestAttackableInRangeEnemyWorkerUnit =
                Broodwar->getBestUnit(
                    getBestEnemyThreatUnitLambda,
                    IsEnemy && IsVisible && IsDetected && Exists && IsWorker &&
                    [&u](Unit& tmpUnit) { return u->canAttack(PositionOrUnit(tmpUnit)) && u->isInWeaponRange(tmpUnit); },
                    u->getPosition(),
                    std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + std::max(Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()), Broodwar->self()->weaponMaxRange(u->getType().airWeapon())));

            if (bestAttackableInRangeEnemyWorkerUnit)
            {
                const BWAPI::Unit oldOrderTarget = u->getTarget();
                if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != bestAttackableInRangeEnemyWorkerUnit)
                {
                    u->attack(bestAttackableInRangeEnemyWorkerUnit);
                }
                continue;
            }

            // Continue statement to avoid sending out mutalisks if we are making guardians and
            // don't have enough guardians yet, because we don't want to kamikaze the mutalisks.
            if (u->getType() == BWAPI::UnitTypes::Zerg_Mutalisk &&
                allUnitCount[UnitTypes::Zerg_Guardian] <= 8 &&
                Broodwar->self()->deadUnitCount(UnitTypes::Zerg_Mutalisk) > 0 &&
                (!startBaseAuto ||
                 u->getDistance(startBaseAuto) < 256))
            {
                continue;
            }

            // Defend base if necessary, e.g. against worker rush, but base race rather than defend
            // if we have no workers left.
            if (allUnitCount[BWAPI::UnitTypes::Zerg_Drone] + numWorkersTrainedThisFrame + allUnitCount[BWAPI::UnitTypes::Zerg_Extractor] > 0 &&
                Broodwar->getFrameCount() < transitionOutOfFourPoolFrameCountThresh &&
                supplyUsed < 60)
            {
                BWAPI::Unit defenceAttackTargetUnit = nullptr;
                if (shouldDefend && workerAttackTargetUnit && u->canAttack(PositionOrUnit(workerAttackTargetUnit)))
                {
                    defenceAttackTargetUnit = workerAttackTargetUnit;
                }
                else if (Broodwar->self()->deadUnitCount(BWAPI::UnitTypes::Zerg_Drone) > 0 &&
                         startBase != nullptr)
                {
                    // Defend my base (even if have to return all the way to my base) if my workers or a building
                    // are threatened e.g. by an enemy worker rush.
                    defenceAttackTargetUnit =
                        Broodwar->getBestUnit(
                            getBestEnemyThreatUnitLambda,
                            IsEnemy && IsVisible && IsDetected && Exists &&
                            CanAttack &&
                            !IsBuilding &&
                            [&u](Unit& tmpUnit)
                            {
                                return u->canAttack(PositionOrUnit(tmpUnit));
                            },
                            startBase->getPosition(),
                            896);
                }
    
                if (defenceAttackTargetUnit && u->canAttack(PositionOrUnit(defenceAttackTargetUnit)))
                {
                    const BWAPI::Unit oldOrderTarget = u->getTarget();
                    if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != defenceAttackTargetUnit)
                    {
                        u->attack(defenceAttackTargetUnit);
                    }
                    continue;
                }
            }

            // Attack enemy lifted buildings.
            if (u->getType().airWeapon() != BWAPI::WeaponTypes::None)
            {
                const BWAPI::Unit closestAttackableEnemyLiftedBuildingUnit =
                    u->getClosestUnit(
                        IsEnemy && IsVisible && Exists && IsLifted &&
                        [&u](Unit& tmpUnit) { return u->canAttack(PositionOrUnit(tmpUnit)); } );

                if (closestAttackableEnemyLiftedBuildingUnit)
                {
                    const BWAPI::Unit oldOrderTarget = u->getTarget();
                    if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != closestAttackableEnemyLiftedBuildingUnit)
                    {
                        u->attack(closestAttackableEnemyLiftedBuildingUnit);
                    }
                    continue;
                }
            }

            // We ignore stolen gas, at least until a time near when we plan to make an extractor.
            const Unit closestEnemyUnliftedBuildingAnywhere =
                u->getClosestUnit(
                    IsEnemy && IsVisible && Exists && IsBuilding && !IsLifted &&
                    isNotStolenGas);

            const BWAPI::Position closestEnemyUnliftedBuildingAnywherePos =
                closestEnemyUnliftedBuildingAnywhere ? closestEnemyUnliftedBuildingAnywhere->getPosition() : BWAPI::Positions::Unknown;

            if (closestEnemyUnliftedBuildingAnywhere)
            {
                // Distance multiplier is arbitrary - the value seems to result in ok movement behaviour.
                if (u->getDistance(closestEnemyUnliftedBuildingAnywhere) <=
                    (int) (std::max(std::max(Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()),
                                             Broodwar->self()->weaponMaxRange(u->getType().airWeapon())), 256) * 1))
                {
                    const BWAPI::Unit bestAttackableEnemyWorkerUnit =
                        Broodwar->getBestUnit(
                            getBestEnemyThreatUnitLambda,
                            IsEnemy && IsVisible && IsDetected && Exists && IsWorker &&
                            [&u, &closestEnemyUnliftedBuildingAnywhere](Unit& tmpUnit)
                            {
                                return u->canAttack(PositionOrUnit(tmpUnit)) &&
                                    tmpUnit->getDistance(u) <= (int) (224 + 32) &&
                                    tmpUnit->getDistance(closestEnemyUnliftedBuildingAnywhere) <= Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()) + 224 &&
                                    tmpUnit->getClosestUnit(Exists && GetPlayer == Broodwar->self(), (int) (224)) != nullptr;
                            },
                            u->getPosition(),
                            std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + 224 + 32);

                    if (bestAttackableEnemyWorkerUnit)
                    {
                        const BWAPI::Unit oldOrderTarget = u->getTarget();
                        if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != bestAttackableEnemyWorkerUnit)
                        {
                            u->attack(bestAttackableEnemyWorkerUnit);
                        }
                        continue;
                    }

                    const BWAPI::Unit bestAttackableInRangeEnemyTacticalUnit =
                        Broodwar->getBestUnit(
                            getBestEnemyThreatUnitLambda,
                            IsEnemy && IsVisible && IsDetected && Exists &&
                            !IsWorker &&
                            (CanAttack ||
                             GetType == BWAPI::UnitTypes::Terran_Bunker ||
                             GetType == BWAPI::UnitTypes::Protoss_High_Templar ||
                             GetType == BWAPI::UnitTypes::Zerg_Defiler ||
                             GetType == BWAPI::UnitTypes::Protoss_Dark_Archon ||
                             GetType == BWAPI::UnitTypes::Terran_Science_Vessel ||
                             GetType == BWAPI::UnitTypes::Zerg_Queen ||
                             GetType == BWAPI::UnitTypes::Protoss_Shuttle ||
                             GetType == BWAPI::UnitTypes::Terran_Dropship ||
                             GetType == BWAPI::UnitTypes::Protoss_Observer ||
                             GetType == BWAPI::UnitTypes::Zerg_Overlord ||
                             GetType == BWAPI::UnitTypes::Terran_Medic ||
                             GetType == BWAPI::UnitTypes::Terran_Nuclear_Silo ||
                             GetType == BWAPI::UnitTypes::Zerg_Nydus_Canal /*||
                             // TODO: re-enable Terran_Comsat_Station after add any
                             // logic to produce cloaked units.
                             GetType == BWAPI::UnitTypes::Terran_Comsat_Station*/) &&
                            [&u](Unit& tmpUnit) { return u->canAttack(PositionOrUnit(tmpUnit)) && u->isInWeaponRange(tmpUnit); },
                            u->getPosition(),
                            std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + std::max(Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()), Broodwar->self()->weaponMaxRange(u->getType().airWeapon())));

                    if (bestAttackableInRangeEnemyTacticalUnit)
                    {
                        const BWAPI::Unit oldOrderTarget = u->getTarget();
                        if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != bestAttackableInRangeEnemyTacticalUnit)
                        {
                            u->attack(bestAttackableInRangeEnemyTacticalUnit);
                        }
                        continue;
                    }

                    // Distance multiplier is arbitrary - the value seems to result in ok movement behaviour.
                    // Less than for closestAttackableEnemyThreatUnit because we would slightly prefer to attack
                    // closer enemy units that can't retaliate than further away ones that can.
                    const BWAPI::Unit bestAttackableEnemyTacticalUnit =
                        Broodwar->getBestUnit(
                            getBestEnemyThreatUnitLambda,
                            IsEnemy && IsVisible && IsDetected && Exists &&
                            !IsWorker &&
                            (CanAttack ||
                             GetType == BWAPI::UnitTypes::Terran_Bunker ||
                             GetType == BWAPI::UnitTypes::Protoss_High_Templar ||
                             GetType == BWAPI::UnitTypes::Zerg_Defiler ||
                             GetType == BWAPI::UnitTypes::Protoss_Dark_Archon ||
                             GetType == BWAPI::UnitTypes::Terran_Science_Vessel ||
                             GetType == BWAPI::UnitTypes::Zerg_Queen ||
                             GetType == BWAPI::UnitTypes::Protoss_Shuttle ||
                             GetType == BWAPI::UnitTypes::Terran_Dropship ||
                             GetType == BWAPI::UnitTypes::Protoss_Observer ||
                             GetType == BWAPI::UnitTypes::Zerg_Overlord ||
                             GetType == BWAPI::UnitTypes::Terran_Medic ||
                             GetType == BWAPI::UnitTypes::Terran_Nuclear_Silo ||
                             GetType == BWAPI::UnitTypes::Zerg_Nydus_Canal /*||
                             // TODO: re-enable Terran_Comsat_Station after add any
                             // logic to produce cloaked units.
                             GetType == BWAPI::UnitTypes::Terran_Comsat_Station*/) &&
                            [&u, &closestEnemyUnliftedBuildingAnywhere](Unit& tmpUnit)
                            {
                                return
                                    u->canAttack(PositionOrUnit(tmpUnit)) &&
                                    (tmpUnit == closestEnemyUnliftedBuildingAnywhere ||
                                     tmpUnit->getDistance(closestEnemyUnliftedBuildingAnywhere) <=
                                         (!tmpUnit->isFlying() ? Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()) : Broodwar->self()->weaponMaxRange(u->getType().airWeapon()))
                                         + 224);
                            },
                            u->getPosition(),
                            std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + (int) (std::max(std::max(Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()),
                                                     Broodwar->self()->weaponMaxRange(u->getType().airWeapon())),
                                            96)
                                   * 1));

                    if (bestAttackableEnemyTacticalUnit)
                    {
                        const BWAPI::Unit oldOrderTarget = u->getTarget();
                        if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != bestAttackableEnemyTacticalUnit)
                        {
                            u->attack(bestAttackableEnemyTacticalUnit);
                        }
                        continue;
                    }

                    // Attack buildings if in range. May be useful against wall-ins.
                    // Commenting-out for the time being.
                    /*const BWAPI::Unit bestAttackableInRangeEnemyNonWorkerUnit =
                        Broodwar->getBestUnit(
                            getBestEnemyThreatUnitLambda,
                            IsEnemy && IsVisible && IsDetected && Exists && !IsWorker &&
                            [&u](Unit& tmpUnit) { return u->canAttack(PositionOrUnit(tmpUnit)) && u->isInWeaponRange(tmpUnit); },
                            u->getPosition(),
                            std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + std::max(Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()), Broodwar->self()->weaponMaxRange(u->getType().airWeapon())));
                    if (bestAttackableInRangeEnemyNonWorkerUnit)
                    {
                        const BWAPI::Unit oldOrderTarget = u->getTarget();
                        if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != bestAttackableInRangeEnemyNonWorkerUnit)
                        {
                            u->attack(bestAttackableInRangeEnemyNonWorkerUnit);
                        }
                        continue;
                    }*/

                    // Distance multiplier should be the same as for closestEnemyUnliftedBuildingAnywhere.
                    const BWAPI::Unit bestAttackableEnemyNonWorkerUnit =
                        Broodwar->getBestUnit(
                            getBestEnemyThreatUnitLambda,
                            IsEnemy && IsVisible && IsDetected && Exists && !IsWorker &&
                            [&u, &closestEnemyUnliftedBuildingAnywhere](Unit& tmpUnit)
                            {
                                return
                                    u->canAttack(PositionOrUnit(tmpUnit)) &&
                                    (tmpUnit == closestEnemyUnliftedBuildingAnywhere ||
                                     tmpUnit->getDistance(closestEnemyUnliftedBuildingAnywhere) <=
                                         (!tmpUnit->isFlying() ? Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()) : Broodwar->self()->weaponMaxRange(u->getType().airWeapon()))
                                         + 224);
                            },
                            u->getPosition(),
                            std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + (int) (std::max(std::max(Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()),
                                                     Broodwar->self()->weaponMaxRange(u->getType().airWeapon())),
                                            256)
                                   * 1));
                    if (bestAttackableEnemyNonWorkerUnit)
                    {
                        const BWAPI::Unit oldOrderTarget = u->getTarget();
                        if (u->isIdle() || oldOrderTarget == nullptr || oldOrderTarget != bestAttackableEnemyNonWorkerUnit)
                        {
                            u->attack(bestAttackableEnemyNonWorkerUnit);
                        }
                        continue;
                    }
                }

                if (isSpeedlingBuildOrder &&
                    (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost) != Broodwar->self()->getMaxUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost) ||
                     (isEnemyXimp && Broodwar->getFrameCount() < 5300)))
                {
                    continue;
                }

                if (closestEnemyUnliftedBuildingAnywherePos != BWAPI::Positions::Unknown && closestEnemyUnliftedBuildingAnywherePos != BWAPI::Positions::None)
                {
                    if (u->canRightClick(PositionOrUnit(closestEnemyUnliftedBuildingAnywherePos)))
                    {
                        // Dunno if rightClick'ing rather than moving is ever beneficial in these scenarios
                        // or whether it is possible to be able to do one but not the other, but let's prefer
                        // rightClick'ing over moving just in case (although it would probably only possibly
                        // matter if the command optimization option level is zero).
                        u->rightClick(closestEnemyUnliftedBuildingAnywherePos);
                        continue;
                    }

                    if (u->canMove())
                    {
                        u->move(closestEnemyUnliftedBuildingAnywherePos);
                        continue;
                    }
                }
            }

            if (isSpeedlingBuildOrder &&
                (Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost) != Broodwar->self()->getMaxUpgradeLevel(BWAPI::UpgradeTypes::Metabolic_Boost) ||
                 (isEnemyXimp && Broodwar->getFrameCount() < 5300)))
            {
                continue;
            }

            std::vector<BWAPI::Position> targetPositions;
            // Block to restrict scope of variables.
            {
                BWAPI::Position targetPos = BWAPI::Positions::Unknown;
                int closestDist = std::numeric_limits<int>::max();
                for (const BWAPI::Position pos : lastKnownEnemyUnliftedBuildingsAnywherePosSet)
                {
                    const int dist = u->getDistance(pos);
                    if (dist < closestDist)
                    {
                        targetPos = pos;
                        closestDist = dist;
                    }
                }

                if (targetPos != BWAPI::Positions::Unknown)
                {
                    targetPositions.push_back(targetPos);
                }
            }

            if (targetPositions.empty() && !enemyStartPositions.empty())
            {
                // For simplicity, let's only attack the first enemy (rather than randomizing to pick one & remembering
                // what we picked for future frames or possibly splitting our army).
                // TODO: skip allies?
                targetPositions.push_back(*enemyStartPositions.begin());
            }

            if (targetPositions.empty() && probableEnemyStartPos != BWAPI::Positions::Unknown)
            {
                targetPositions.push_back(probableEnemyStartPos);
            }

            if (targetPositions.empty() && !unscoutedOtherStartPositions.empty())
            {
                const int tmpX = (int) u->getClientInfo(scoutingTargetPosXInd);
                const int tmpY = (int) u->getClientInfo(scoutingTargetPosYInd);
                if ((tmpX != 0 || tmpY != 0) &&
                    unscoutedOtherStartPositions.find(Position(tmpX, tmpY)) != unscoutedOtherStartPositions.end())
                {
                    targetPositions.push_back(Position(tmpX, tmpY));
                }
                else
                {
                    for (const BWAPI::Position pos : unscoutedOtherStartPositions)
                    {
                        targetPositions.push_back(pos);
                    }
                }
            }

            if (targetPositions.empty())
            {
                const int tmpX = (int) u->getClientInfo(scoutingTargetPosXInd);
                const int tmpY = (int) u->getClientInfo(scoutingTargetPosYInd);
                // If en-route to a position that isn't visible or isn't clear then continue going there.
                // Occasionally re-randomize late-game cos the unit may not have a path to get there.
                if ((tmpX != 0 || tmpY != 0) &&
                    Broodwar->getFrameCount() % (60 * 24) >= 6 &&
                    (!Broodwar->isVisible(TilePosition(Position(tmpX, tmpY))) ||
                     !Broodwar->getUnitsOnTile(TilePosition(Position(tmpX, tmpY)), IsEnemy && IsVisible && Exists && IsBuilding && !IsLifted).empty()))
                {
                    targetPositions.push_back(Position(tmpX, tmpY));
                }
                else
                {
                    // Target a random position - preferably one that is not visible.
                    BWAPI::Position pos;
                    for (int i = 0; i < 10; ++i)
                    {
                        pos =
                            Position(rand() % (Broodwar->mapWidth() * BWAPI::TILEPOSITION_SCALE),
                                     rand() % (Broodwar->mapHeight() * BWAPI::TILEPOSITION_SCALE));

                        if (!Broodwar->isVisible(TilePosition(pos)))
                        {
                            break;
                        }
                    }

                    targetPositions.push_back(pos);
                }
            }

            if (!targetPositions.empty())
            {
                BWAPI::Position pos = BWAPI::Positions::None;
                if (targetPositions.size() == 1)
                {
                    pos = targetPositions.front();
                }
                else
                {
                    // Target the closest target position that has less than a certain number of units
                    // assigned to it, or if they all have at least that amount then target the one that
                    // has the least assigned to it.
                    std::sort(
                        targetPositions.begin(),
                        targetPositions.end(),
                        [&u](const BWAPI::Position p1, const BWAPI::Position p2)
                        {
                            return (u->getDistance(p1) < u->getDistance(p2));
                        });
    
                    BWAPI::Position posWithFewestUnits = BWAPI::Positions::None;
                    for (const BWAPI::Position targetPos : targetPositions)
                    {
                        if (numUnitsTargetingPos.find(targetPos) == numUnitsTargetingPos.end() ||
                            numUnitsTargetingPos.at(targetPos) < 3)
                        {
                            pos = targetPos;
                            ++numUnitsTargetingPos[pos];
                            break;
                        }
                        else if (posWithFewestUnits == BWAPI::Positions::None ||
                                 (numUnitsTargetingPos.find(posWithFewestUnits) != numUnitsTargetingPos.end() &&
                                  numUnitsTargetingPos.at(targetPos) < numUnitsTargetingPos.at(posWithFewestUnits)))
                        {
                            posWithFewestUnits = targetPos;
                        }
                    }
    
                    if (pos == BWAPI::Positions::None)
                    {
                        pos = posWithFewestUnits;
                        ++numUnitsTargetingPos[pos];
                    }
                }

                if (u->canRightClick(PositionOrUnit(pos)))
                {
                    // Dunno if rightClick'ing rather than moving is ever beneficial in these scenarios
                    // or whether it is possible to be able to do one but not the other, but let's prefer
                    // rightClick'ing over moving just in case (although it would probably only possibly
                    // matter if the command optimization option level is zero).
                    u->rightClick(pos);
                    u->setClientInfo(pos.x, scoutingTargetPosXInd);
                    u->setClientInfo(pos.y, scoutingTargetPosYInd);
                    continue;
                }

                if (u->canMove())
                {
                    u->move(pos);
                    u->setClientInfo(pos.x, scoutingTargetPosXInd);
                    u->setClientInfo(pos.y, scoutingTargetPosYInd);
                    continue;
                }
            }
        }
        else if (u->getType() == UnitTypes::Zerg_Overlord)
        {
            if (!noCmdPending(u))
            {
                continue;
            }

            bool anEnemyIsTerran = false;
            for (const BWAPI::Player p : Broodwar->enemies())
            {
                if (p->getRace() == BWAPI::Races::Terran)
                {
                    anEnemyIsTerran = true;
                }
            }

            const BWAPI::Position oldTargetPos = u->getTargetPosition();

            // After we have started pulling an overlord back to base, don't send it out again.
            if (oldTargetPos == getPos(Broodwar->self()->getStartLocation(), BWAPI::UnitTypes::Special_Start_Location))
            {
                continue;
            }

            BWAPI::Position targetPos = BWAPI::Positions::Unknown;

            // If we know where an enemy starting location is and an enemy is known to be Terran race
            // then don't risk sending out overlords / return them back to our base for safety.
            // If being attacked then return back to our base for safety.
            // Note: isUnderAttack is misleading / not reliable as mentioned elsewhere in this src but never mind.
            // Return overlords back to our base for safety after a few minutes of in-game time or if we see a risk
            // (or against a Terran, when see any enemy building or roughly when a Terran could get first marine).
            if (!enemyStartPositions.empty() ||
                (anEnemyIsTerran && otherStartPositions.size() == 1) ||
                (anEnemyIsTerran && (!lastKnownEnemyUnliftedBuildingsAnywherePosSet.empty() || (isSpeedlingBuildOrder ? probableEnemyStartPos != BWAPI::Positions::Unknown : Broodwar->getFrameCount() >= 2600))) ||
                u->isUnderAttack() ||
                 Broodwar->getBestUnit(
                     getBestEnemyThreatUnitLambda,
                     IsEnemy && IsVisible && Exists && !IsWorker &&
                     (CanAttack ||
                      // Pull overlords back if we see special buildings like hydra den that are likely to mean
                      // the enemy will produce units that can kill the overlord somehow (e.g. even psi storm).
                      // Not Terran_Barracks because there is already logic based on frame count to cover marines.
                      //GetType == BWAPI::UnitTypes::Terran_Barracks ||
                      GetType == BWAPI::UnitTypes::Zerg_Hydralisk_Den ||
                      GetType == BWAPI::UnitTypes::Protoss_Stargate ||
                      GetType == BWAPI::UnitTypes::Terran_Starport ||
                      GetType == BWAPI::UnitTypes::Terran_Control_Tower ||
                      GetType == BWAPI::UnitTypes::Zerg_Spire ||
                      GetType == BWAPI::UnitTypes::Zerg_Greater_Spire ||
                      GetType == BWAPI::UnitTypes::Protoss_Fleet_Beacon ||
                      GetType == BWAPI::UnitTypes::Protoss_Arbiter_Tribunal ||
                      GetType == BWAPI::UnitTypes::Terran_Science_Facility ||
                      GetType == BWAPI::UnitTypes::Terran_Physics_Lab ||
                      GetType == BWAPI::UnitTypes::Terran_Covert_Ops ||
                      GetType == BWAPI::UnitTypes::Terran_Nuclear_Silo ||
                      GetType == BWAPI::UnitTypes::Protoss_Templar_Archives ||
                      GetType == BWAPI::UnitTypes::Terran_Bunker ||
                      GetType == BWAPI::UnitTypes::Protoss_High_Templar ||
                      GetType == BWAPI::UnitTypes::Zerg_Defiler ||
                      GetType == BWAPI::UnitTypes::Protoss_Dark_Archon ||
                      GetType == BWAPI::UnitTypes::Terran_Science_Vessel ||
                      GetType == BWAPI::UnitTypes::Zerg_Queen ||
                      GetType == BWAPI::UnitTypes::Protoss_Shuttle ||
                      GetType == BWAPI::UnitTypes::Terran_Dropship ||
                      //GetType == BWAPI::UnitTypes::Protoss_Observer ||
                      //GetType == BWAPI::UnitTypes::Zerg_Overlord ||
                      GetType == BWAPI::UnitTypes::Terran_Medic ||
                      GetType == BWAPI::UnitTypes::Terran_Nuclear_Silo ||
                      GetType == BWAPI::UnitTypes::Zerg_Nydus_Canal /*||
                      GetType == BWAPI::UnitTypes::Terran_Comsat_Station*/) &&
                     [&u](Unit& tmpUnit) { return !tmpUnit->getType().canAttack() || tmpUnit->getType().airWeapon() != BWAPI::WeaponTypes::None; },
                     u->getPosition(),
                     std::max(u->getType().dimensionLeft(), std::max(u->getType().dimensionUp(), std::max(u->getType().dimensionRight(), u->getType().dimensionDown()))) + (int) (std::max(std::max(Broodwar->self()->weaponMaxRange(u->getType().groundWeapon()),
                                                 Broodwar->self()->weaponMaxRange(u->getType().airWeapon())),
                                        1024)
                                   * 1)) != nullptr)
            {
                targetPos = getPos(Broodwar->self()->getStartLocation(), BWAPI::UnitTypes::Special_Start_Location);
            }

            if (targetPos == BWAPI::Positions::Unknown)
            {
                if (oldTargetPos != BWAPI::Positions::None && oldTargetPos != BWAPI::Positions::Unknown &&
                    (isSpeedlingBuildOrder ? true : Broodwar->getFrameCount() < (5 * 60 * 24)) &&
                    (!Broodwar->isVisible(TilePosition(oldTargetPos)) ||
                     !Broodwar->getUnitsOnTile(TilePosition(oldTargetPos), IsEnemy && IsVisible && Exists && IsBuilding && !IsLifted).empty()))
                {
                    targetPos = oldTargetPos;
                }
            }

            if (targetPos == BWAPI::Positions::Unknown && !possibleOverlordScoutPositions.empty())
            {
                int closestOtherStartPosDistance = std::numeric_limits<int>::max();
                for (const BWAPI::Position pos : possibleOverlordScoutPositions)
                {
                    const int dist = u->getDistance(pos);
                    if (dist < closestOtherStartPosDistance)
                    {
                        targetPos = pos;
                        closestOtherStartPosDistance = dist;
                    }
                }
            }

            if (targetPos == BWAPI::Positions::Unknown)
            {
                // Commented out randomizing - for now let's return overlords back to our base for safety.
                targetPos = getPos(Broodwar->self()->getStartLocation(), BWAPI::UnitTypes::Special_Start_Location);
                //// Target a random position.
                //targetPos = Position(rand() % (Broodwar->mapWidth() * BWAPI::TILEPOSITION_SCALE),
                //                     rand() % (Broodwar->mapHeight() * BWAPI::TILEPOSITION_SCALE));
            }

            if (u->canRightClick(PositionOrUnit(targetPos)))
            {
                u->rightClick(targetPos);
                possibleOverlordScoutPositions.erase(targetPos);
                continue;
            }

            if (u->canMove())
            {
                u->move(targetPos);
                possibleOverlordScoutPositions.erase(targetPos);
                continue;
            }
        }
    }

    // Mineral gathering commands.
    if (!myFreeGatherers.empty())
    {
        // The first stage assigns free gatherers to free mineral patches near our starting base:
        // Prioritise the mineral patches according to shortest distance to our starting base
        // (note that the set of free mineral patches shrinks while this algorithm is executing,
        // i.e. remove a mineral patch from the set whenever a gatherer is assigned to that patch,
        // to avoid multiple gatherers being assigned to the same mineral patch that was initially free,
        // at least until the next stage),
        // then combinations of free gatherer and mineral patch are prioritised according to shortest
        // total distance from the gatherer to the mineral patch plus mineral patch to our starting base.
        //
        // The second stage simply assigns each remaining gatherer to whatever mineral patch is
        // closest to our starting base. If there are multiple mineral patches nearby our starting base
        // that are the same distance to our starting base then ties are broken by picking the mineral
        // patch closest to the gatherer (i.e. unfortunatley in many cases they will all be assigned
        // to the same mineral patch but never mind).
        if (startBase)
        {
            Unitset& freeMinerals =
                startBase->getUnitsInRadius(
                    256,
                    BWAPI::Filter::IsMineralField &&
                    BWAPI::Filter::Exists &&
                    [&myFreeGatherers, &resourceToGathererMapAuto, &gathererToResourceMapAuto](BWAPI::Unit& tmpUnit)
                    {
                        if (tmpUnit->getResources() <= 0)
                        {
                            return false;
                        }
    
                        std::map<const BWAPI::Unit, BWAPI::Unit>::iterator resourceToGathererMapIter = resourceToGathererMapAuto.find(tmpUnit);
                        if (resourceToGathererMapIter == resourceToGathererMapAuto.end())
                        {
                            return true;
                        }
    
                        BWAPI::Unit& gatherer = resourceToGathererMapIter->second;
                        if (!gatherer->exists() ||
                            myFreeGatherers.contains(gatherer) ||
                            // Commented this out because it was causing workers' paths to cross (inefficient?).
                            //// Override workers that are currently returning from a mineral patch to a depot.
                            //gatherer->getOrder() == BWAPI::Orders::ResetCollision ||
                            //gatherer->getOrder() == BWAPI::Orders::ReturnMinerals ||
                            // Override workers that are not currently gathering minerals.
                            !gatherer->isGatheringMinerals())
                        {
                            return true;
                        }
    
                        std::map<const BWAPI::Unit, BWAPI::Unit>::iterator gathererToResourceMapIter = gathererToResourceMapAuto.find(gatherer);
                        if (gathererToResourceMapIter == gathererToResourceMapAuto.end())
                        {
                            return true;
                        }
    
                        BWAPI::Unit& resource = gathererToResourceMapIter->second;
                        return resource != tmpUnit;
                    });

            // Each element of the set is a mineral.
            struct MineralSet { std::set<const BWAPI::Unit> val; };
            // The key is the total distance from mineral to depot plus distance from gatherer to mineral.
            struct TotCostMap { std::map<int, MineralSet> val; };
            // The key is the distance from mineral to depot.
            struct MineralToDepotCostMap { std::map<int, TotCostMap> val; };
            // The key is the gatherer.
            struct CostMap { std::map<const BWAPI::Unit, MineralToDepotCostMap> val; };
    
            CostMap costMap;
            bool isGatherPossible = false;

            // Fill in the cost map.
            for (auto& mineral : freeMinerals)
            {
                const int mineralToDepotCost = mineral->getDistance(startBase);
                for (auto& gatherer : myFreeGatherers)
                {
                    if (gatherer->canGather(mineral))
                    {
                        costMap.val[gatherer].val[mineralToDepotCost].val[gatherer->getDistance(mineral) + mineralToDepotCost].val.emplace(mineral);
                        isGatherPossible = true;
                    }
                }
            }

            while (isGatherPossible)
            {
                isGatherPossible = false;
                BWAPI::Unit bestGatherer = nullptr;
                BWAPI::Unit bestMineral = nullptr;
                int bestMineralToDepotCost = std::numeric_limits<int>::max();
                int bestTotCost = std::numeric_limits<int>::max();

                for (std::map<const BWAPI::Unit, MineralToDepotCostMap>::iterator gathererIter = costMap.val.begin(); gathererIter != costMap.val.end(); )
                {
                    if (!myFreeGatherers.contains(gathererIter->first))
                    {
                        costMap.val.erase(gathererIter++);
                        continue;
                    }

                    bool isGatherPossibleForGatherer = false;
                    BWAPI::Unit mineral = nullptr;
                    int mineralToDepotCost = std::numeric_limits<int>::max();
                    int totCost = std::numeric_limits<int>::max();

                    for (std::map<int, TotCostMap>::iterator mineralToDepotCostIter = gathererIter->second.val.begin(); mineralToDepotCostIter != gathererIter->second.val.end(); )
                    {
                        bool isGatherPossibleForMineralToDepotCost = false;
                        if (mineralToDepotCostIter->first > bestMineralToDepotCost)
                        {
                            // Need to assume it is still possible.
                            isGatherPossibleForGatherer = true;
                            break;
                        }
    
                        for (std::map<int, MineralSet>::iterator totCostIter = mineralToDepotCostIter->second.val.begin(); totCostIter != mineralToDepotCostIter->second.val.end(); )
                        {
                            bool isGatherPossibleForTotCost = false;
                            if (mineralToDepotCostIter->first == bestMineralToDepotCost && totCostIter->first > bestTotCost)
                            {
                                // Need to assume it is still possible.
                                isGatherPossibleForMineralToDepotCost = true;
                                break;
                            }
    
                            for (std::set<const BWAPI::Unit>::iterator mineralIter = totCostIter->second.val.begin(); mineralIter != totCostIter->second.val.end(); )
                            {
                                if (!freeMinerals.contains(*mineralIter))
                                {
                                    totCostIter->second.val.erase(mineralIter++);
                                    continue;
                                }

                                mineral = *mineralIter;
                                isGatherPossibleForTotCost = true;
                                mineralToDepotCost = mineralToDepotCostIter->first;
                                totCost = totCostIter->first;
                                break;
                            }

                            if (!isGatherPossibleForTotCost)
                            {
                                mineralToDepotCostIter->second.val.erase(totCostIter++);
                                continue;
                            }
                            else
                            {
                                isGatherPossibleForMineralToDepotCost = true;
                                break;
                            }
                        }

                        if (!isGatherPossibleForMineralToDepotCost)
                        {
                            gathererIter->second.val.erase(mineralToDepotCostIter++);
                            continue;
                        }
                        else
                        {
                            isGatherPossibleForGatherer = true;
                            break;
                        }
                    }

                    if (!isGatherPossibleForGatherer)
                    {
                        myFreeGatherers.erase(gathererIter->first);
                        costMap.val.erase(gathererIter++);
                        continue;
                    }
                    else
                    {
                        if (mineralToDepotCost < bestMineralToDepotCost || (mineralToDepotCost == bestMineralToDepotCost && totCost < bestTotCost))
                        {
                            isGatherPossible = true;
                            bestGatherer = gathererIter->first;
                            bestMineral = mineral;
                            bestMineralToDepotCost = mineralToDepotCost;
                            bestTotCost = totCost;
                        }

                        ++gathererIter;
                        continue;
                    }
                }

                if (isGatherPossible)
                {
                    bestGatherer->gather(bestMineral);

                    myFreeGatherers.erase(bestGatherer);
                    freeMinerals.erase(bestMineral);
                
                    if (gathererToResourceMap.find(bestGatherer) != gathererToResourceMap.end() && resourceToGathererMap.find(gathererToResourceMap.at(bestGatherer)) != resourceToGathererMap.end() && resourceToGathererMap.at(gathererToResourceMap.at(bestGatherer)) == bestGatherer)
                    {
                        resourceToGathererMap.erase(gathererToResourceMap.at(bestGatherer));
                    }
                
                    resourceToGathererMap[bestMineral] = bestGatherer;
                    gathererToResourceMap[bestGatherer] = bestMineral;
                    continue;
                }
            }

            for (auto& u : myFreeGatherers)
            {
                BWAPI::Unit mineralField = nullptr;
                mineralField = Broodwar->getBestUnit(
                    [&u, &startBaseAuto](const BWAPI::Unit& bestSoFarUnit, const BWAPI::Unit& curUnit)
                    {
                        if (curUnit->getDistance(startBaseAuto) != bestSoFarUnit->getDistance(startBaseAuto))
                        {
                            return curUnit->getDistance(startBaseAuto) < bestSoFarUnit->getDistance(startBaseAuto) ? curUnit : bestSoFarUnit;
                        }

                        return u->getDistance(curUnit) < u->getDistance(bestSoFarUnit) ? curUnit : bestSoFarUnit;
                    },
                    BWAPI::Filter::IsMineralField &&
                    BWAPI::Filter::Exists &&
                    [&u](BWAPI::Unit& tmpUnit)
                    {
                        return tmpUnit->getResources() > 0 && u->canGather(tmpUnit);
                    },
                    startBaseAuto->getPosition(),
                    std::max(startBaseAuto->getType().dimensionLeft(), std::max(startBaseAuto->getType().dimensionUp(), std::max(startBaseAuto->getType().dimensionRight(), startBaseAuto->getType().dimensionDown()))) + 256);

                if (mineralField == nullptr)
                {
                    mineralField = startBase->getClosestUnit(
                        BWAPI::Filter::IsMineralField &&
                        BWAPI::Filter::Exists &&
                        [&u](BWAPI::Unit& tmpUnit)
                        {
                            return tmpUnit->getResources() > 0 && u->canGather(tmpUnit);
                        });
                }

                if (mineralField)
                {
                    u->gather(mineralField);
                
                    if (gathererToResourceMap.find(u) != gathererToResourceMap.end() && resourceToGathererMap.find(gathererToResourceMap.at(u)) != resourceToGathererMap.end() && resourceToGathererMap.at(gathererToResourceMap.at(u)) == u)
                    {
                        resourceToGathererMap.erase(gathererToResourceMap.at(u));
                    }
                
                    resourceToGathererMap[mineralField] = u;
                    gathererToResourceMap[u] = mineralField;
                    continue;
                }
            }
        }
    }

    // Update client info for each of my units (so can check it in future frames).
    for (auto& u : myUnits)
    {
        if (u->exists() && u->isCompleted() && u->getType() != BWAPI::UnitTypes::Zerg_Larva && u->getType() != BWAPI::UnitTypes::Zerg_Egg)
        {
            const int newX = u->getPosition().x;
            const int newY = u->getPosition().y;
            if ((int) u->getClientInfo(posXInd) != newX || (int) u->getClientInfo(posYInd) != newY)
            {
                u->setClientInfo(Broodwar->getFrameCount(), frameLastChangedPosInd);
            }
    
            u->setClientInfo(newX, posXInd);
            u->setClientInfo(newY, posYInd);

            if (u->isAttacking())
            {
                u->setClientInfo(Broodwar->getFrameCount(), frameLastAttackingInd);
            }

            if (u->isAttackFrame())
            {
                u->setClientInfo(Broodwar->getFrameCount(), frameLastAttackFrameInd);
            }

            if (u->isStartingAttack())
            {
                u->setClientInfo(Broodwar->getFrameCount(), frameLastStartingAttackInd);
            }
    
            if (u->getType().isWorker() && u->isCarryingMinerals())
            {
                u->setClientInfo(wasJustCarryingMineralsTrueVal, wasJustCarryingMineralsInd);
            }
        }
    }
}

void ZZZKBotAIModule::onSendText(std::string text)
{
    // Send the text to the game if it is not being processed.
    Broodwar->sendText("%s", text.c_str());

    // Make sure to use %s and pass the text as a parameter,
    // otherwise you may run into problems when you use the %(percent) character!
}

void ZZZKBotAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
}

void ZZZKBotAIModule::onPlayerLeft(BWAPI::Player player)
{
}

void ZZZKBotAIModule::onNukeDetect(BWAPI::Position target)
{
    // Check if the target is a valid position
    if (target)
    {
        // if so, print the location of the nuclear strike target
        Broodwar << "Nuclear Launch Detected at " << target << std::endl;
    }
    else
    {
        Broodwar << "Nuclear Launch Detected at unknown position" << std::endl;
    }

    // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void ZZZKBotAIModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void ZZZKBotAIModule::onUnitEvade(BWAPI::Unit unit)
{
}

void ZZZKBotAIModule::onUnitShow(BWAPI::Unit unit)
{
}

void ZZZKBotAIModule::onUnitHide(BWAPI::Unit unit)
{
}

void ZZZKBotAIModule::onUnitCreate(BWAPI::Unit unit)
{
    if (Broodwar->isReplay())
    {
        // if we are in a replay, then we will print out the build order of the structures
        if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
        {
            int seconds = Broodwar->getFrameCount()/24;
            int minutes = seconds/60;
            seconds %= 60;
            Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
        }
    }
}

void ZZZKBotAIModule::onUnitDestroy(BWAPI::Unit unit)
{
}

void ZZZKBotAIModule::onUnitMorph(BWAPI::Unit unit)
{
    if (Broodwar->isReplay())
    {
        // if we are in a replay, then we will print out the build order of the structures
        if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
        {
            int seconds = Broodwar->getFrameCount()/24;
            int minutes = seconds/60;
            seconds %= 60;
            Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
        }
    }
}

void ZZZKBotAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void ZZZKBotAIModule::onSaveGame(std::string gameName)
{
    Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ZZZKBotAIModule::onUnitComplete(BWAPI::Unit unit)
{
}
