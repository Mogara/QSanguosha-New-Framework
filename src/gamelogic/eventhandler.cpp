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

EventHandler::EventHandler()
    : m_defaultPriority(0)
{
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

QMap<ServerPlayer *, Event> EventHandler::triggerable(GameLogic *logic, EventType event, ServerPlayer *owner, QVariant &data) const
{
    QMap<ServerPlayer *, Event> result;
    EventList events = triggerable(logic, event, owner, data, owner);
    foreach (const Event &d, events)
        result.insertMulti(owner, d);
    return result;
}

EventList EventHandler::triggerable(GameLogic *logic, EventType event, ServerPlayer *owner, QVariant &data, ServerPlayer *invoker) const
{
    Q_UNUSED(logic)
    Q_UNUSED(event)
    Q_UNUSED(data)
    Q_UNUSED(invoker)

    EventList events;
    if (triggerable(owner))
        events << Event(this, owner);

    return events;
}

bool EventHandler::cost(GameLogic *logic, EventType event, ServerPlayer *target, QVariant &data, ServerPlayer *invoker) const
{
    Q_UNUSED(logic)
    Q_UNUSED(event)
    Q_UNUSED(target)
    Q_UNUSED(data)
    Q_UNUSED(invoker)
    return true;
}

bool EventHandler::effect(GameLogic *logic, EventType event, ServerPlayer *target, QVariant &data, ServerPlayer *invoker) const
{
    Q_UNUSED(logic)
    Q_UNUSED(event)
    Q_UNUSED(target)
    Q_UNUSED(data)
    Q_UNUSED(invoker)
    return false;
}
