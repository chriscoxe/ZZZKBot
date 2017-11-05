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
// (LGPL) version 3).

#pragma once
#include <BWAPI.h>
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
    const std::string onEndUpdateSignifier = "onEnd";

    int enemyPlayerID = -1;
    std::string enemyWriteFilePath;

    std::time_t timerAtGameStart = std::time(nullptr);
};
