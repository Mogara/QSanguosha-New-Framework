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
#include <QSet>
#include <QMap>

class GameLogic;
class Player;

class EventHandler
{
public:
    QSet<EventType> events() const { return m_events; }

    QString name() const { return m_name; }
    int priority() const { return m_priority; }
    bool isEquipSkill() const { return m_equipSkill; }

    virtual EventList triggerable(GameLogic *logic, EventType event, const QVariant &data, ServerPlayer *player = nullptr) const;
    virtual bool onCost(GameLogic *logic, EventType event, EventPtr eventPtr, QVariant &data, ServerPlayer *player = nullptr) const;
    virtual bool effect(GameLogic *logic, EventType event, EventPtr eventPtr, QVariant &data, ServerPlayer *player = nullptr) const;


protected:
    EventHandler();

    QSet<EventType> m_events;
    QString m_name;
    int m_priority;
    bool m_equipSkill;

};

#endif // EVENTHANDLER_H
