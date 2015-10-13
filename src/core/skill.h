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

#include <QString>
#include <QList>

class Skill
{
public:
    enum Frequency
    {
        NotFrequent,
        Frequent,
        Compulsory,
        Limited,
        Wake
    };

    enum Type
    {
        InvalidType,
        TriggerType,
        ViewAsType
    };

    Skill(const QString &name);
    virtual ~Skill();

    QString name() const { return m_name; }
    Type type() const { return m_type; }
    Frequency frequency() const { return m_frequency; }
    QList<const Skill *> subskills() const { return m_subskills; }
    bool isLordSkill() const { return m_lordSkill; }
    const Skill *topSkill() const;

protected:
    void addSubskill(Skill *subskill);

    QString m_name;
    Type m_type;
    Frequency m_frequency;
    QList<const Skill *> m_subskills;
    bool m_lordSkill;
    const Skill *m_topSkill;
};

class TriggerSkill : public Skill, public EventHandler
{
public:
    TriggerSkill(const QString &name);

    bool triggerable(ServerPlayer *owner) const override;
    bool cost(GameLogic *, EventType, ServerPlayer *, QVariant &data, ServerPlayer *invoker) const override;
};

class Card;

class ViewAsSkill : public Skill
{
public:
    ViewAsSkill(const QString &name);

    //Check if the card can be selected
    virtual bool viewFilter(const Card *card, const Player *self) const = 0;

    //Returns the generated new card
    virtual Card *viewAs(const QList<Card *> &cards, const Player *self) const = 0;

    //An empty pattern means it requires all kinds of cards
    virtual bool isAvailable(const Player *self, const QString &pattern) const;
};

#endif // SKILL_H
