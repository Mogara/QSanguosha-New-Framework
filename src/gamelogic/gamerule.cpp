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

#include <QThread>

QMap<EventType, GameRule::Callback> GameRule::m_callbacks;

GameRule::GameRule(GameLogic *logic)
    : m_logic(logic)
{
    m_events << GameStart;
    m_events << TurnStart << PhaseProceeding;
}

bool GameRule::triggerable(ServerPlayer *) const
{
    return true;
}

bool GameRule::effect(GameLogic *logic, EventType event, ServerPlayer *current, QVariant &data, Player *) const
{
    if (logic->skipGameRule())
        return false;

    Callback func = m_callbacks[event];
    if (func)
        (this->*func)(current, data);
    else
        throw GameFinish;

    return false;
}

void GameRule::onGameStart(ServerPlayer *current, QVariant &) const
{
    current->drawCards(4);
}

void GameRule::onTurnStart(ServerPlayer *current, QVariant &) const
{
    current->setTurnCount(current->turnCount() + 1);
    if (!current->faceUp())
        current->setFaceUp(true);
    else
        current->play();
}

void GameRule::onPhaseProceeding(ServerPlayer *current, QVariant &) const
{
    m_logic->delay(500);
    switch (current->phase()) {
    case Player::Draw: {
        int num = 2;
        QVariant data(num);
        m_logic->trigger(DrawNCards, current, data);
        num = data.toInt();
        if (num > 0)
            current->drawCards(num);
        data = num;
        m_logic->trigger(AfterDrawNCards, current, data);
        break;
    }
    case Player::Play: {
        while (current->isAlive()) {
            CardUseStruct use;
            current->activate(use);
            if (use.card != nullptr)
                m_logic->useCard(use);
            else
                break;
        }
        break;
    }
    case Player::Discard: {
        int maxCardNum = current->hp();
        QVariant data(maxCardNum);
        m_logic->trigger(CountMaxCardNum, current, data);
        maxCardNum = data.toInt();
        int discardNum = current->handcardNum() - maxCardNum;
        //@to-do: discarding cards if (discardNum > 0)
        break;
    }
    default:;
    }
}

#define ADD_GAMERULE(name) m_callbacks[name] = &GameRule::on##name
void GameRule::Init()
{
    ADD_GAMERULE(GameStart);
    ADD_GAMERULE(TurnStart);
    ADD_GAMERULE(PhaseProceeding);
}
C_INITIALIZE_CLASS(GameRule)
