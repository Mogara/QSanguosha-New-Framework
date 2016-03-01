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
#include "datavalue.h"

#include <QObject>

#include <QString>
#include <QList>
#include <QJSValue>

class Card;

class Skill : public QObject
{
    Q_OBJECT
    friend class General;
    Q_PROPERTY(uint id READ id)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(Type type READ type)
    Q_PROPERTY(int subtype READ subtype)
    Q_PROPERTY(Frequency frequency MEMBER m_frequency READ frequency WRITE setFrequency)
    Q_PROPERTY(QList<const Skill *> subskills READ subskills)
    Q_PROPERTY(bool isLordSkill READ isLordSkill)
    Q_PROPERTY(const Skill *topSkill READ topSkill)


public:
    enum Frequency
    {
        NotFrequent,
        Frequent,
        Compulsory,
        Limited,
        Wake
    };
    Q_ENUM(Frequency)

    enum Type
    {
        InvalidType,
        TriggerType,
        ViewAsType,
        CardModType
    };
    Q_ENUM(Type)

    Q_INVOKABLE explicit Skill(const QString &name);
    virtual ~Skill();

    uint id() const { return m_id; }
    QString name() const { return m_name; }
    Type type() const { return m_type; }
    int subtype() const { return m_subtype; }
    Frequency frequency() const { return m_frequency; }
    QList<const Skill *> subskills() const { return m_subskills; }
    bool isLordSkill() const { return m_lordSkill; }
    const Skill *topSkill() const;

    Q_INVOKABLE void addSubskill(Skill *subskill);

    void setFrequency(Frequency frequency);

    QJSValue skillFuncs() const;
    static Skill *newSkill(const QString &type, const QString &name);

protected:
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
    Q_OBJECT

public:
    Q_INVOKABLE explicit TriggerSkill(const QString &name);

    using Skill::name;
    using Skill::m_name;

    EventList triggerable(GameLogic *logic, EventType event, const QObject *data, ServerPlayer *player /* = nullptr */) const override;
    virtual bool cost(GameLogic *logic, EventType event, EventPtr eventPtr, QObject *data, ServerPlayer *player /* = nullptr */) const;
    bool effect(GameLogic *logic, EventType event, const EventPtr eventPtr, QObject *data, ServerPlayer *player /* = nullptr */) const override;

    bool onCost(GameLogic *logic, EventType event, EventPtr eventPtr, QObject *data, ServerPlayer *player = nullptr) const final override;
};

class StatusSkill : public TriggerSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit StatusSkill(const QString &name);

    void validate(ServerPlayer *target) const;
    void invalidate(ServerPlayer *target) const;
    bool isValid(ServerPlayer *target) const;

    bool effect(GameLogic *logic, EventType event, const EventPtr eventPtr, QObject *data, ServerPlayer *player) const final override;
};

/*
class MasochismSkill : public TriggerSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MasochismSkill(const QString &name);

    int triggerable(GameLogic *logic, ServerPlayer *player, const DamageValue *damage) const;
    void effect(GameLogic *logic, ServerPlayer *player, const EventPtr eventPtr, const DamageValue *damage) const;

    EventList triggerable(GameLogic *logic, EventType event, const QObject *data, ServerPlayer *player = nullptr) const final override;
    bool effect(GameLogic *logic, EventType event, const EventPtr eventPtr, QObject *data, ServerPlayer *player) const final override;
};
*/

class ViewAsSkill : public Skill
{
    Q_OBJECT

public:
    enum Subtype
    {
        ConvertType,
        ProactiveType
    };
    Q_ENUM(Subtype)

    Q_INVOKABLE explicit ViewAsSkill(const QString &name);

    bool isAvailable(const Player *self, const QString &pattern) const;
    virtual bool viewFilter(const QList<const Card *> &selected, const Card *card, const Player *self, const QString &pattern) const;
    virtual Card *viewAs(const QList<Card *> &cards, const Player *self) const;

    //Check if selected cards are valid
    bool isValid(const QList<Card *> &cards, const Player *self, const QString &pattern) const;
};

/*
class OneCardViewAsSkill : public ViewAsSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit OneCardViewAsSkill(const QString &name);

    bool viewFilter(const Card *card, const Player *self, const QString &pattern) const;
    Card *viewAs(Card *card, const Player *self) const;

    bool viewFilter(const QList<const Card *> &selected, const Card *card, const Player *self, const QString &pattern) const final override;
    Card *viewAs(const QList<Card *> &cards, const Player *self) const final override;
};
*/

class ProactiveSkill : public ViewAsSkill
{
    Q_OBJECT


public:
    Q_INVOKABLE explicit ProactiveSkill(const QString &name);

    //Check if selected players are valid
    bool isValid(const QList<ServerPlayer *> &targets, ServerPlayer *source) const;
    bool isValid(const QList<const Player *> &targets, const Player *source) const;
    //Check if selected cards are valid
    bool isValid(const QList<Card *> &cards, const Player *source, const QString &pattern) const;

    bool cardFeasible(const QList<const Card *> &selected, const Player *source) const;
    bool cardFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &pattern) const;
    bool playerFeasible(const QList<const Player *> &selected, const Player *source) const;
    bool playerFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
    bool cost(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const;
    void effect(GameLogic *logic, ServerPlayer *from, const QList<ServerPlayer *> &to, const QList<Card *> &cards) const;

    bool viewFilter(const QList<const Card *> &selected, const Card *card, const Player *source, const QString &pattern) const final override;
    Card *viewAs(const QList<Card *> &cards, const Player *source) const final override;
};

/*
class RetrialSkill : public ProactiveSkill
{
    Q_OBJECT
    Q_PROPERTY(bool isReplace MEMBER m_isReplace)

private:
    class Timing;
public:
    Q_INVOKABLE explicit RetrialSkill(const QString &name, bool replace = false);

protected:
    bool m_isReplace;
};

class ProactiveSkillTiming : public TriggerSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ProactiveSkillTiming(const QString &name, const ProactiveSkill *parent);

    bool cost(GameLogic *logic, EventType event, EventPtr eventPtr, QObject *data, ServerPlayer *player / * = nullptr * /) const final override;
    bool effect(GameLogic *logic, EventType event, const EventPtr eventPtr, QObject *data, ServerPlayer *player / * = nullptr * /) const final override;

private:
    const ProactiveSkill *m_parent;
};
*/

class CardModSkill : public Skill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CardModSkill(const QString &name);

    bool targetFilter(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
    int extraDistanceLimit(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
    int extraMaxTargetNum(const Card *card, const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
    int extraUseNum(const Card *card, const Player *player) const;
};

#endif // SKILL_H
