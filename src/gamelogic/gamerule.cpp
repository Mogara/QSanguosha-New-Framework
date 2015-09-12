/********************************************************************
    Copyright (c) 2013-2015 - Mogara

    This file is part of QSanguosha.

    This game engine is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3.0
    of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the LICENSE file for more details.

    Mogara
*********************************************************************/

#include "gamelogic.h"
#include "gamerule.h"
#include "serverplayer.h"

GameRule::GameRule(GameLogic *logic)
    : m_logic(logic)
{
    m_events << GameStart;
    m_events << TurnStart << PhaseProceeding << PhaseEnd << PhaseChanging;
}

bool GameRule::triggerable(ServerPlayer *) const
{
    return true;
}

bool GameRule::effect(GameLogic *logic, EventType event, ServerPlayer *current, QVariant &data, Player *) const
{
    if (logic->skipGameRule())
        return false;

    if (event == GameStart) {
        onGameStart(current);
    } else {
        throw GameFinish;
    }

    return false;
}

void GameRule::onGameStart(ServerPlayer *current) const
{
    current->drawCards(4);
}
