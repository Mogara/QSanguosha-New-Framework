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

#include "event.h"
#include "eventhandler.h"
#include "serverplayer.h"

EventHandler::EventHandler()
    : m_priority(0)
    , m_equipSkill(false)
{
}

EventHandler::~EventHandler()
{
}

EventList EventHandler::triggerable(GameLogic *, EventType, const QVariant &, ServerPlayer *) const
{
    return EventList();
}

bool EventHandler::onCost(GameLogic *, EventType, EventPtr, QVariant &, ServerPlayer *) const
{
    return true;
}

bool EventHandler::effect(GameLogic *, EventType, EventPtr, QVariant &, ServerPlayer *) const
{
    return false;
}
