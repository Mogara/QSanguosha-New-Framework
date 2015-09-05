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

#include "eventhandler.h"
#include "player.h"

EventHandler::EventHandler()
{
    m_defaultPriority = 0;
}

EventHandler::~EventHandler()
{
}

int EventHandler::priority() const
{
    return m_defaultPriority;
}

int EventHandler::priority(EventType event) const
{
    return m_priorityMap.contains(event) ? m_priorityMap.value(event) : m_defaultPriority;
}

bool EventHandler::triggerable(Player *owner) const
{
    return owner != NULL && owner->isAlive() && owner->hasSkill(this);
}

QMap<Player *, Event> EventHandler::triggerable(GameLogic *logic, EventType event, Player *owner, QVariant &data) const
{
    QMap<Player *, Event> result;
    QList<Event> Events = triggerable(logic, event, owner, data, owner);
    foreach (const Event &d, Events)
        result.insertMulti(owner, d);
    return result;
}

QList<Event> EventHandler::triggerable(GameLogic *logic, EventType event, Player *owner, QVariant &data, Player *invoker) const
{
    Q_UNUSED(logic)
    Q_UNUSED(event)
    Q_UNUSED(data)
    Q_UNUSED(invoker)

    QList<Event> Events;
    if (triggerable(owner))
        Events << Event(this, owner);

    return Events;
}

bool EventHandler::cost(GameLogic *logic, EventType event, Player *target, QVariant &data, Player *invoker) const
{
    Q_UNUSED(logic)
    Q_UNUSED(event)
    Q_UNUSED(target)
    Q_UNUSED(data)
    Q_UNUSED(invoker)
    return true;
}

bool EventHandler::effect(GameLogic *logic, EventType event, Player *target, QVariant &data, Player *invoker) const
{
    Q_UNUSED(logic)
    Q_UNUSED(event)
    Q_UNUSED(target)
    Q_UNUSED(data)
    Q_UNUSED(invoker)
    return false;
}
