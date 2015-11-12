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
        ViewAsType,
        CardModType
    };

    Skill(const QString &name);
    virtual ~Skill();

    QString name() const { return m_name; }
    Type type() const { return m_type; }
    int subtype() const { return m_subtype; }
    Frequency frequency() const { return m_frequency; }
    QList<const Skill *> subskills() const { return m_subskills; }
    bool isLordSkill() const { return m_lordSkill; }
    const Skill *topSkill() const;

protected:
    void addSubskill(Skill *subskill);

    QString m_name;
    Type m_type;
    int m_subtype;
    Frequency m_frequency;
    bool m_lordSkill;

private:
    QList<const Skill *> m_subskills;
    const Skill *m_topSkill;
};

class TriggerSkill : public Skill, public EventHandler
{
public:
    TriggerSkill(const QString &name);

    bool triggerable(ServerPlayer *owner) const override;
    bool onCost(GameLogic *logic, EventType event, ServerPlayer *owner, QVariant &data, ServerPlayer *invoker) const final override;

    virtual bool cost(GameLogic *logic, EventType event, ServerPlayer *owner, QVariant &data, ServerPlayer *invoker) const;

protected:
    void setFrequency(Frequency frequency);
};

class Card;

template<typename T>
static bool CheckAvailability(const Player *self)
{
    T *card = new T(T::NoSuit, 0);
    bool result = card->isAvailable(self);
    delete card;
    return result;
}

class ViewAsSkill : public Skill
{
public:
    enum Subtype{
        ConvertType,
        ProactiveType
    };

    ViewAsSkill(const QString &name);

    //An empty pattern means it's the playing phase
    virtual bool isAvailable(const Player *self, const QString &pattern) const;

    //Check if the card can be selected
    virtual bool viewFilter(const QList<const Card *> &selected, const Card *card, const Player *self, const QString &pattern) const = 0;

    //Returns the generated new card
    virtual Card *viewAs(const QList<Card *> &cards, const Player *self) const = 0;
};

class OneCardViewAsSkill : public ViewAsSkill
{
public:
    OneCardViewAsSkill(const QString &name);

    bool viewFilter(const QList<const Card *> &selected, const Card *card, const Player *self, const QString &pattern) const final override;
    Card *viewAs(const QList<Card *> &cards, const Player *self) const final override;

    virtual bool viewFilter(const Card *card, const Player *self, const QString &pattern) const = 0;
    virtual Card *viewAs(Card *card, const Player *self) const = 0;
};

class ProactiveSkill : public ViewAsSkill
{
public:
    ProactiveSkill(const QString &name);

    //An empty pattern means it's the playing phase
    virtual bool isAvailable(const Player *self, const QString &pattern) const;

    //Check if cards are feasible
    virtual bool cardFeasible(const QList<const Card *> &selected, const Player *source) const;

    //Check if the card can be selected
    virtual bool cardFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &pattern) const;

    //Check if the target players are feasible
    virtual bool playerFeasible(const QList<const Player *> &selected, const Player *source) const;

    //Check if a player can be selected
    virtual bool playerFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;

    virtual void effect(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const = 0;

    bool viewFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &pattern) const final override;
    Card *viewAs(const QList<Card *> &cards, const Player *source) const final override;
};

class CardModSkill : public Skill
{
public:
    CardModSkill(const QString &name);

    virtual bool targetFilter(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
    virtual int extraDistanceLimit(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
    virtual int extraMaxTargetNum(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
};

#endif // SKILL_H
