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
#include "gamelogic.h"
#include "eventhandler.h"
#include "serverplayer.h"
#include "skill.h"


bool Event::operator <(const Event &arg2) const // the operator < for sorting the invoke order.
{
    //  we sort firstly according to the priority, then the seat of invoker, at last whether it is a skill of an equip.
    if (!isValid() || !arg2.isValid())
        return false;

    if (eh->priority() != arg2.eh->priority())
        return eh->priority() > arg2.eh->priority();

    if (invoker != arg2.invoker)
        return ServerPlayer::sortByActionOrder(invoker, arg2.invoker);

    return eh->isEquipSkill() && !eh->isEquipSkill();
}

bool Event::sameSkill(const Event &arg2) const // it only judge the eh, the invoker and the owner. it don't judge the real skill target because it is chosen by the skill invoker
{
    return eh == arg2.eh && owner == arg2.owner && invoker == arg2.invoker;
}

bool Event::sameTimingWith(const Event &arg2) const // used to judge 2 skills has the same timing. only 2 structs with the same priority and the same invoker and the same "whether or not it is a skill of equip"
{
    if (!isValid() || !arg2.isValid())
        return false;

    return eh->priority() == arg2.eh->priority() && invoker == arg2.invoker && eh->isEquipSkill() == arg2.eh->isEquipSkill();
}

Event::Event(GameLogic *logic, const EventHandler *eh /*= nullptr*/, ServerPlayer *owner /*= nullptr*/, ServerPlayer *invoker /*= nullptr*/, bool isCompulsory /*= false*/, ServerPlayer *preferredTarget /*= nullptr*/)
    : logic(logic), eh(eh), owner(owner), invoker(invoker), isCompulsory(isCompulsory), preferredTarget(preferredTarget), triggered(false)
{
}

Event::Event(const Event &arg2)
    : QObject()
{
    logic = arg2.logic;
    eh = arg2.eh;
    owner = arg2.owner;
    invoker = arg2.invoker;
    isCompulsory = arg2.isCompulsory;
    preferredTarget = arg2.preferredTarget;
    targets = arg2.targets;
    triggered = arg2.triggered;
    tag = arg2.tag;
}

Event &Event::operator =(const Event &arg2)
{
    logic = arg2.logic;
    eh = arg2.eh;
    owner = arg2.owner;
    invoker = arg2.invoker;
    isCompulsory = arg2.isCompulsory;
    preferredTarget = arg2.preferredTarget;
    targets = arg2.targets;
    triggered = arg2.triggered;
    tag = arg2.tag;

    return *this;
}

bool Event::isValid() const // validity check
{
    return logic != nullptr && eh != nullptr;
}

bool Event::preferredTargetLess(const Event &arg2) const
{
    if (eh == arg2.eh && owner == arg2.owner && invoker == arg2.invoker) {
        // we compare preferred target to ensure the target selected is in the order of seat only in the case that 2 skills are the same
        if (preferredTarget != nullptr && arg2.preferredTarget != nullptr)
            return ServerPlayer::sortByActionOrder(preferredTarget, arg2.preferredTarget);
    }

    return false;
}

QVariant Event::toVariant() const
{
    if (!isValid())
        return QVariant();

    QVariantMap ob;
    if (eh)
        ob["skill"] = eh->name();
    if (owner)
        ob["owner"] = owner->id();
    if (invoker)
        ob["invoker"] = invoker->id();
    if (preferredTarget) {
        ob["preferredtarget"] = preferredTarget->id();
        ob["preferredtargetseat"] = logic->allPlayers().indexOf(preferredTarget);
    }
    return ob;
}

QStringList Event::toList() const
{
    QStringList l;
    if (!isValid())
        l << QString() << QString() << QString() << QString();
    else {
        std::function<void(const QObject *)> insert = [&l](const QObject *item) {
            if (item)
                l << item->objectName();
            else
                l << QString();
        };

        if (eh != nullptr)
            l << eh->name();
        else
            l << QString();

        insert(owner);
        insert(invoker);
        insert(preferredTarget);
    }

    return l;
}
