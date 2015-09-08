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

Skill::Skill(const QString &name, QObject *parent)
    : QObject(parent)
    , m_frequency(NotFrequent)
    , m_lordSkill(false)
{
    setObjectName(name);
}

void Skill::addSubskill(Skill *skill)
{
    skill->setParent(this);
    m_subskills << skill;
}

QList<const Skill *> Skill::subskills() const
{
    QList<const Skill *> subskills;
    subskills.reserve(m_subskills.length());
    foreach (const Skill *skill, m_subskills)
        subskills << skill;
    return subskills;
}


TriggerSkill::TriggerSkill(const QString &name, QObject *parent)
    : Skill(name, parent)
{

}
