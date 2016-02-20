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

#include "event.h"
#include "eventhandler.h"
#include "structs.h"

#include <QString>
#include <QList>

class Skill
{
    friend class General;

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

    explicit Skill(const QString &name);
    virtual ~Skill();

    uint id() const { return m_id; }
    QString name() const { return m_name; }
    Type type() const { return m_type; }
    int subtype() const { return m_subtype; }
    Frequency frequency() const { return m_frequency; }
    QList<const Skill *> subskills() const { return m_subskills; }
    bool isLordSkill() const { return m_lordSkill; }
    const Skill *topSkill() const;

protected:
    void addSubskill(Skill *subskill);

    uint m_id;
    QString m_name;
    Type m_type;
    int m_subtype;
    Frequency m_frequency;
    bool m_lordSkill;

private:
    QList<const Skill *> m_subskills;
    const Skill *m_topSkill;

private:
    C_DISABLE_COPY(Skill);
};

class TriggerSkill : public Skill, public EventHandler
{
public:
    explicit TriggerSkill(const QString &name);

    // QString name() const { return Skill::name(); }
    using Skill::name;

    bool triggerable(ServerPlayer *owner) const;
    EventList triggerable(GameLogic *logic, EventType event, const QVariant &data, ServerPlayer *player = nullptr) const override;
    bool onCost(GameLogic *logic, EventType event, EventPtr eventPtr, QVariant &data, ServerPlayer *player = nullptr) const final override;

    virtual bool cost(GameLogic *logic, EventType event, EventPtr eventPtr, QVariant &data, ServerPlayer *player = nullptr) const;

protected:
    void setFrequency(Frequency frequency);
};

class StatusSkill : public TriggerSkill
{
public:
    explicit StatusSkill(const QString &name);

    virtual void validate(ServerPlayer *target) const = 0;
    virtual void invalidate(ServerPlayer *target) const = 0;
    virtual bool isValid(ServerPlayer *target) const;

    bool effect(GameLogic *logic, EventType event, EventPtr eventPtr, QVariant &data, ServerPlayer *player) const final override;
};

class MasochismSkill : public TriggerSkill
{
public:
    explicit MasochismSkill(const QString &name);

    virtual int triggerable(GameLogic *logic, ServerPlayer *player, const DamageStruct &damage) const = 0;
    EventList triggerable(GameLogic *logic, EventType event, const QVariant &data, ServerPlayer *player = nullptr) const final override;

    virtual void effect(GameLogic *logic, ServerPlayer *player, EventPtr eventPtr, const DamageStruct &damage) const = 0;
    bool effect(GameLogic *logic, EventType event, EventPtr eventPtr, QVariant &data, ServerPlayer *player) const final override;
};

class Card;

template<typename T>
static bool CheckAvailability(const Player *self)
{
    T *card = new T(T::UndeterminedSuit, T::UndeterminedNumber);
    bool result = card->isAvailable(self);
    delete card;
    return result;
}

class ViewAsSkill : public Skill
{
public:
    enum Subtype
    {
        ConvertType,
        ProactiveType
    };

    explicit ViewAsSkill(const QString &name);

    //An empty pattern means it's the playing phase
    virtual bool isAvailable(const Player *self, const QString &pattern) const;

    //Check if the card can be selected
    virtual bool viewFilter(const QList<const Card *> &selected, const Card *card, const Player *self, const QString &pattern) const = 0;

    //Returns the generated new card
    virtual Card *viewAs(const QList<Card *> &cards, const Player *self) const = 0;

    //Check if selected cards are valid
    bool isValid(const QList<Card *> &cards, const Player *self, const QString &pattern) const;
};

class OneCardViewAsSkill : public ViewAsSkill
{
public:
    explicit OneCardViewAsSkill(const QString &name);

    bool viewFilter(const QList<const Card *> &selected, const Card *card, const Player *self, const QString &pattern) const final override;
    Card *viewAs(const QList<Card *> &cards, const Player *self) const final override;

    virtual bool viewFilter(const Card *card, const Player *self, const QString &pattern) const = 0;
    virtual Card *viewAs(Card *card, const Player *self) const = 0;
};

class ProactiveSkill : public ViewAsSkill
{
public:
    explicit ProactiveSkill(const QString &name);

    //An empty pattern means it's the playing phase
    virtual bool isAvailable(const Player *self, const QString &pattern) const;

    //Check if cards are feasible
    virtual bool cardFeasible(const QList<const Card *> &selected, const Player *source) const;

    //Check if the card can be selected
    virtual bool cardFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &pattern) const;

    //Check if selected cards are valid
    bool isValid(const QList<Card *> &cards, const Player *source, const QString &pattern) const;

    //Check if the target players are feasible
    virtual bool playerFeasible(const QList<const Player *> &selected, const Player *source) const;

    //Check if a player can be selected
    virtual bool playerFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;

    //Check if selected players are valid
    bool isValid(const QList<ServerPlayer *> &targets, ServerPlayer *source) const;
    bool isValid(const QList<const Player *> &targets, const Player *source) const;

    virtual bool cost(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const;
    virtual void effect(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const;

    bool viewFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &pattern) const final override;
    Card *viewAs(const QList<Card *> &cards, const Player *source) const final override;
};

class RetrialSkill : public ProactiveSkill
{
private:
    class Timing;
public:
    explicit RetrialSkill(const QString &name, bool isReplace = false);

    bool cost(GameLogic *, ServerPlayer *, const QList<ServerPlayer *> &, const QList<Card *> &) const final override { return false; }
    void effect(GameLogic *, ServerPlayer *, const QList<ServerPlayer *> &, const QList<Card *> &) const final override {}
    
private:
    bool m_isReplace;
};

class ProactiveSkillTiming : public TriggerSkill
{
public:
    explicit ProactiveSkillTiming(const QString &name, const ProactiveSkill *parent);

    bool cost(GameLogic *logic, EventType event, EventPtr eventPtr, QVariant &data, ServerPlayer *player /* = nullptr */) const final override;
    bool effect(GameLogic *logic, EventType event, EventPtr eventPtr, QVariant &data, ServerPlayer *player /* = nullptr */) const final override;

private:
    const ProactiveSkill *m_parent;
};

class CardModSkill : public Skill
{
public:
    explicit CardModSkill(const QString &name);

    virtual bool targetFilter(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
    virtual int extraDistanceLimit(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
    virtual int extraMaxTargetNum(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
    virtual int extraUseNum(const Card *card, const Player *player) const;
};

#endif // SKILL_H
