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

Event::Event()
    : handler(nullptr)
    , owner(nullptr)
{
}

Event::Event(const EventHandler *handler)
    : handler(handler)
    , owner(nullptr)
{
}

Event::Event(const EventHandler *handler, ServerPlayer *owner)
    : handler(handler)
    , owner(owner)
{
}

EventList::EventList(const Event &e)
{
    append(e);
}

EventMap::EventMap(const Event &e)
{
    insert(e.owner, e);
}

EventMap::EventMap(const EventList &events)
{
    foreach(const Event &e, events)
        insert(e.owner, e);
}
