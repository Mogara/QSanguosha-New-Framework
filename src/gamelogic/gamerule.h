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
    GameRule();

    virtual void prepareToStart(GameLogic *logic) const = 0;

    bool triggerable(ServerPlayer *owner) const override;
    bool effect(GameLogic *logic, EventType event, ServerPlayer *current, QVariant &data, ServerPlayer *) const override;

protected:
    typedef void (*Callback)(GameLogic *, ServerPlayer *, QVariant &);
    QMap<EventType, Callback> m_callbacks;
};

#endif // GAMERULE_H
