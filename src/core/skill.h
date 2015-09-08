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

#ifndef SKILL_H
#define SKILL_H

#include "eventhandler.h"

#include <QObject>
#include <QList>

class Skill : public QObject
{
    Q_OBJECT

public:
    enum Frequency
    {
        NotFrequent,
        Frequent,
        Compulsory,
        Limited,
        Wake
    };

    Skill(const QString &name, QObject *parent = 0);

    Frequency frequency() const { return m_frequency; }

    void addSubskill(Skill *skill);
    QList<const Skill *> subskills() const;

    bool isLordSkill() const { return m_lordSkill; }

private:
    Frequency m_frequency;
    QList<Skill *> m_subskills;
    bool m_lordSkill;
};

class TriggerSkill : public Skill, public EventHandler
{
    Q_OBJECT

public:
    TriggerSkill(const QString &name, QObject *parent = 0);
};

#endif // SKILL_H
