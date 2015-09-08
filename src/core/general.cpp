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

#include "general.h"
#include "skill.h"

General::General(const QString &name, const QString &kingdom, int maxHp, Gender gender)
    : m_name(name)
    , m_kingdom(kingdom)
    , m_maxHp(maxHp)
    , m_gender(gender)
    , m_lord(false)
    , m_hidden(false)
    , m_neverShown(false)
    , m_headExtraMaxHp(0)
    , m_deputyExtraMaxHp(0)
{
}

General::~General()
{
    foreach (Skill *skill, m_skills)
        delete skill;
}

bool General::isCompanionWith(const General *general) const
{
    if (m_companions.contains(general->name()))
        return true;

    return kingdom() == general->kingdom() && (isLord() || general->isLord());
}
