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

#include "clientskill.h"
#include "skill.h"

ClientSkill::ClientSkill(const Skill *skill, QObject *parent)
    : QObject(parent)
    , m_skill(skill)
    , m_enabled(false)
{
}

uint ClientSkill::id() const
{
    return m_skill->id();
}

QString ClientSkill::name() const
{
    return m_skill->name();
}

QString ClientSkill::type() const
{
    switch (m_skill->frequency()) {
    case Skill::NotFrequent:
    case Skill::Frequent:
        return "proactive";
    case Skill::Wake:
    case Skill::Limited:
        return "limited";
    case Skill::Compulsory:
    default:
        //@to-do: battle array skill
        return "compulsory";
    }
}

void ClientSkill::setEnabled(bool enabled)
{
    m_enabled = enabled;
    emit enabledChanged();
}
