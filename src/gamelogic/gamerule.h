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

#ifndef GAMERULE_H
#define GAMERULE_H

#include <QMap>
#include <cglobal.h>

#include "eventhandler.h"

class GameLogic;
class Skill;

class GameRule : public EventHandler
{
public:
    GameRule(GameLogic *logic);

    bool triggerable(ServerPlayer *) const override;
    bool effect(GameLogic *logic, EventType event, ServerPlayer *current, QVariant &data, ServerPlayer *) const override;

protected:
    void addPlayerSkill(const Skill *skill);

    typedef void (GameRule::*Callback)(ServerPlayer *, QVariant &) const;
    void onGameStart(ServerPlayer *current, QVariant &) const;
    void onTurnStart(ServerPlayer *current, QVariant &) const;
    void onPhaseProceeding(ServerPlayer *current, QVariant &) const;
    void onPhaseEnd(ServerPlayer *current, QVariant &) const;
    void onAfterHpReduced(ServerPlayer *victim, QVariant &data) const;
    void onAskForPeach(ServerPlayer *current, QVariant &data) const;
    void onAskForPeachDone(ServerPlayer *victim, QVariant &data) const;

    GameLogic *m_logic;
    static QMap<EventType, Callback> m_callbacks;
    C_DECLARE_INITIALIZER(GameRule)
};

#endif // GAMERULE_H
