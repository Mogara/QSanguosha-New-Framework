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
class GameLogic;

#include <QList>
#include <QVariantMap>
#include <QSharedPointer>

struct Event
{
    explicit Event(GameLogic *logic, const EventHandler *eh = nullptr, ServerPlayer *owner = nullptr, ServerPlayer *invoker = nullptr, bool isCompulsory = false, ServerPlayer *preferredTarget = nullptr);

    GameLogic *logic;
    const EventHandler *eh; // the EventHandler
    ServerPlayer *owner; // skill owner. 2 structs with the same skill and skill owner are treated as of a same skill.
    ServerPlayer *invoker; // skill invoker. When invoking skill, we sort firstly according to the priority, then the seat of invoker, at last weather it is a skill of an equip.
    bool isCompulsory; // judge the skill is compulsory or not. It is set in the skill's triggerable
    ServerPlayer *preferredTarget; // the preferred target of a certain skill

    QList<ServerPlayer *> targets; // skill targets.
    bool triggered; // judge whether the skill is triggered

    QVariantMap tag; // used to add a tag to the struct. useful for skills like Tieqi and Liegong to save a QVariantList for assisting to assign targets

    bool operator <(const Event &arg2) const; // the operator < for sorting the invoke order.
    bool sameSkill(const Event &arg2) const; // the operator ==. it only judge the skill name, the skill invoker, and the skill owner. it don't judge the skill target because it is chosen by the skill invoker
    bool sameTimingWith(const Event &arg2) const; // used to judge 2 skills has the same timing. only 2 structs with the same priority and the same invoker and the same "whether or not it is a skill of equip"
    bool isValid() const; // validity check
    bool preferredTargetLess(const Event &arg2) const;

    QVariant toVariant() const;
    QStringList toList() const;
};

typedef QList<Event> EventList;
typedef QSharedPointer<Event> EventPtr;
typedef QList<QSharedPointer<Event> > EventPtrList;


#endif // EVENT_H

