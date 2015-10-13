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

#include "skill.h"
#include "serverplayer.h"

Skill::Skill(const QString &name)
    : m_name(name)
    , m_type(InvalidType)
    , m_frequency(NotFrequent)
    , m_lordSkill(false)
    , m_topSkill(nullptr)
{
}

Skill::~Skill()
{
    foreach (const Skill *subskill, m_subskills)
        delete subskill;
}

const Skill *Skill::topSkill() const
{
    if (m_topSkill == nullptr)
        return this;
    return m_topSkill;
}

void Skill::addSubskill(Skill *subskill)
{
    subskill->m_topSkill = this;
    m_subskills << subskill;
}

TriggerSkill::TriggerSkill(const QString &name)
    : Skill(name)
{
    m_type = TriggerType;
    m_defaultPriority = 1;
}

bool TriggerSkill::triggerable(ServerPlayer *owner) const
{
    return owner != nullptr && owner->isAlive() && owner->hasSkill(this);
}

bool TriggerSkill::cost(GameLogic *, EventType, ServerPlayer *, QVariant &data, ServerPlayer *invoker) const
{
    return invoker->askToInvokeSkill(this, data);
}

ViewAsSkill::ViewAsSkill(const QString &name)
    : Skill(name)
{
    m_type = ViewAsType;
}

bool ViewAsSkill::isAvailable(const Player *self, const QString &pattern) const
{
    Q_UNUSED(self)
    return pattern.isEmpty();
}
