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
    EventHandler();
    virtual ~EventHandler() = 0;

    QList<EventType> events() const { return m_events; }

    QString name() const { return m_name; }

    int priority() const;
    int priority(EventType event) const;

    bool isCompulsory() const { return m_compulsory; }

    virtual bool triggerable(ServerPlayer *owner) const = 0;
    virtual QMap<ServerPlayer *, Event> triggerable(GameLogic *logic, EventType event, ServerPlayer *owner, QVariant &data) const;
    virtual EventList triggerable(GameLogic *logic, EventType event, ServerPlayer *owner, QVariant &data, ServerPlayer *invoker) const;
    virtual bool onCost(GameLogic *logic, EventType event, ServerPlayer *target, QVariant &data, ServerPlayer *invoker = nullptr) const;
    virtual bool effect(GameLogic *logic, EventType event, ServerPlayer *target, QVariant &data, ServerPlayer *invoker = nullptr) const;

protected:
    QList<EventType> m_events;
    QString m_name;
    int m_defaultPriority;
    QMap<EventType, int> m_priorityMap;
    bool m_compulsory;
};

#endif // EVENTHANDLER_H
