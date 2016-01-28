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

#ifndef EVENT_H
#define EVENT_H

class EventHandler;
class ServerPlayer;

#include <QList>
#include <QMultiMap>

struct Event
{
    Event();
    Event(const EventHandler *handler);
    Event(const EventHandler *handler, ServerPlayer *owner);

    bool isValid() { return handler != nullptr; }

    const EventHandler *handler;
    ServerPlayer *owner;
    QList<ServerPlayer *> to;
};

class EventList : public QList<Event>
{
public:
    EventList() {}
    EventList(const Event &e);
};

class EventMap : public QMultiMap<ServerPlayer *, Event>
{
public:
    EventMap() {}
    EventMap(const Event &e);
    EventMap(const EventList &events);
};

#endif // EVENT_H

