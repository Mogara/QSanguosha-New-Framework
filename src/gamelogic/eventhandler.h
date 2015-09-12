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

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "eventtype.h"
#include "event.h"

#include <QVariant>
#include <QList>
#include <QMap>

class GameLogic;
class Player;

class EventHandler
{
public:
    enum Frequency
    {
        NotFrequent,
        Frequent,
        Compulsory,
        Limited,
        Wake
    };

    EventHandler();
    virtual ~EventHandler() = 0;

    QList<EventType> events() const { return m_events; }

    int priority() const;
    int priority(EventType event) const;

    Frequency frequency() const { return m_frequency; }

    virtual bool triggerable(ServerPlayer *owner) const;
    virtual QMap<ServerPlayer *, Event> triggerable(GameLogic *logic, EventType event, ServerPlayer *owner, QVariant &data) const;
    virtual QList<Event> triggerable(GameLogic *logic, EventType event, ServerPlayer *owner, QVariant &data, Player *invoker) const;
    virtual bool cost(GameLogic *logic, EventType event, ServerPlayer *target, QVariant &data, Player *invoker = NULL) const;
    virtual bool effect(GameLogic *logic, EventType event, ServerPlayer *target, QVariant &data, Player *invoker = NULL) const;

protected:
    QList<EventType> m_events;
    int m_defaultPriority;
    QMap<EventType, int> m_priorityMap;
    Frequency m_frequency;
};

#endif // EVENTHANDLER_H
