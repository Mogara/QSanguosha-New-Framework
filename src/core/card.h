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

#ifndef CARD_H
#define CARD_H

#include <QObject>
#include <QList>
#include <QSet>

class GameLogic;
class Player;

#include "structs.h"

class Card : public QObject
{
    Q_OBJECT

    Q_ENUMS(Suit)
    Q_ENUMS(Color)

    Q_PROPERTY(uint id READ id)
    Q_PROPERTY(uint effectiveId READ effectiveId)
    Q_PROPERTY(QString suit READ suitString WRITE setSuitString)
    Q_PROPERTY(int number READ number WRITE setNumber)
    Q_PROPERTY(QString color READ colorString WRITE setColorString)
    Q_PROPERTY(QString type READ typeString)
    Q_PROPERTY(bool transferable READ isTransferable WRITE setTransferable)
    //Q_PROPERTY(QString skillName READ skillName WRITE setSkillName)

    friend class Package;

public:
    enum Suit
    {
        NoSuit,

        Spade,
        Heart,
        Club,
        Diamond
    };

    enum Color
    {
        NoColor,

        Red,
        Black
    };

    enum Type
    {
        InvalidType,
        BasicType,
        TrickType,
        EquipType
    };

    enum Number
    {
        InfinityNum = 1000
    };

    Card(Suit suit = NoSuit, int number = 0);
    virtual Card *clone() const;

    uint id() const { return m_id; }
    bool isVirtual() const { return id() == 0; }
    uint effectiveId() const;

    void setSuit(Suit suit) { m_suit = suit; }
    Suit suit() const;
    void setSuitString(const QString &suit);
    QString suitString() const;

    void setNumber(int number) { m_number = number; }
    int number() const;

    void setColor(Color color) { m_color = color; }
    Color color() const;
    void setColorString(const QString &color);
    QString colorString() const;

    void setType(Type type) { m_type = type; }
    Type type() const { return m_type; }
    int subtype() const { return m_subtype; }
    QString typeString() const;

    void addSubcard(Card *card);
    void setSubcards(const QList<Card *> &cards) { m_subcards = cards; }
    QList<Card *> subcards() const { return m_subcards; }

    Card *realCard();
    const Card *realCard() const;
    QList<Card *> realCards();
    QList<const Card *> realCards() const;

    void setSkill(const Skill *skill) { m_skill = skill; }
    const Skill *skill() const { return m_skill; }

    void addFlag(const QString &flag) { m_flags.insert(flag); }
    void removeFlag(const QString &flag) { m_flags.remove(flag); }
    bool hasFlag(const QString &flag) const { return m_flags.contains(flag); }
    void clearFlags() { m_flags.clear(); }

    void setTransferable(bool transferable) { m_transferable = transferable; }
    bool isTransferable() const { return m_transferable; }

    bool canRecast() const { return m_canRecast; }
    int useLimit() const;
    int useLimit(const Player *source) const;
    int maxTargetNum() const;
    int minTargetNum() const;
    int distanceLimit() const;

    bool isTargetFixed() const { return m_targetFixed; }
    virtual bool targetFeasible(const QList<const Player *> &selected, const Player *source) const;
    virtual bool targetFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const;
    virtual bool isAvailable(const Player *source) const;

    virtual void onUse(GameLogic *logic, CardUseStruct &use);
    virtual void use(GameLogic *logic, CardUseStruct &use);
    virtual void onEffect(GameLogic *logic, CardEffectStruct &effect);
    virtual void effect(GameLogic *logic, CardEffectStruct &effect);
    virtual void complete(GameLogic *logic);

protected:
    uint m_id;
    Suit m_suit;
    int m_number;
    Color m_color;
    Type m_type;
    int m_subtype;

    bool m_transferable;
    bool m_canRecast;
    int m_useLimit;
    int m_maxTargetNum;
    int m_minTargetNum;
    int m_distanceLimit;
    bool m_targetFixed;

    const Skill *m_skill;
    QList<Card *> m_subcards;
    QSet<QString> m_flags;
};

class BasicCard : public Card
{
    Q_OBJECT

public:
    BasicCard(Suit suit, int number);
};

class TrickCard : public Card
{
    Q_OBJECT

public:
    enum SubType
    {
        //Immediate Types
        GlobalEffectType,
        AreaOfEffectType,
        SingleTargetType,
        DamageSpreadType,

        //Delayed Types
        DelayedType
    };

    TrickCard(Suit suit, int number);

    void onEffect(GameLogic *logic, CardEffectStruct &effect) override;
    virtual bool isNullifiable(const CardEffectStruct &effect) const;
};

class Skill;

class EquipCard : public Card
{
    Q_OBJECT
    Q_ENUMS(SubType)

public:
    enum SubType
    {
        WeaponType,
        ArmorType,
        DefensiveHorseType,
        OffensiveHorseType,
        TreasureType
    };

    EquipCard(Suit suit, int number);

    void onUse(GameLogic *logic, CardUseStruct &use) override;
    void use(GameLogic *logic, CardUseStruct &use) override;
    void complete(GameLogic *) override;

    Skill *skill() const { return m_skill; }

protected:
    Skill *m_skill;
};

class GlobalEffect : public TrickCard
{
    Q_OBJECT

public:
    GlobalEffect(Card::Suit suit, int number);

    void onUse(GameLogic *logic, CardUseStruct &use) override;
};

class AreaOfEffect : public TrickCard
{
    Q_OBJECT

public:
    AreaOfEffect(Suit suit, int number);

    void onUse(GameLogic *logic, CardUseStruct &use) override;
};

class SingleTargetTrick : public TrickCard
{
    Q_OBJECT

public:
    SingleTargetTrick(Suit suit, int number);
};

class DelayedTrick : public TrickCard
{
    Q_OBJECT

public:
    DelayedTrick(Suit suit, int number);

    bool targetFeasible(const QList<const Player *> &selected, const Player *) const override;
    bool targetFilter(const QList<const Player *> &selected, const Player *toSelect, const Player *source) const override;
    void onUse(GameLogic *logic, CardUseStruct &use) override;
    void use(GameLogic *logic, CardUseStruct &use) override;
    void onEffect(GameLogic *logic, CardEffectStruct &effect) override;
    void effect(GameLogic *logic, CardEffectStruct &effect) override;

    virtual void takeEffect(GameLogic *logic, CardEffectStruct &effect) = 0;

protected:
    QString m_judgePattern;
};

class MovableDelayedTrick : public DelayedTrick
{
    Q_OBJECT

public:
    MovableDelayedTrick(Suit suit, int number);

    void onUse(GameLogic *logic, CardUseStruct &use) override;
    void effect(GameLogic *logic, CardEffectStruct &effect) override;
    void complete(GameLogic *logic);
    bool isAvailable(const Player *player) const override;
};

class Weapon : public EquipCard
{
    Q_OBJECT

public:
    Weapon(Suit suit, int number);

    int attackRange() const;

protected:
    int m_attackRange;
};

class Armor : public EquipCard
{
    Q_OBJECT

public:
    Armor(Suit suit, int number);
};

class Horse : public EquipCard
{
    Q_OBJECT

public:
    Horse(Suit suit, int number);

    Card *clone() const override;
};

class OffensiveHorse : public Horse
{
    Q_OBJECT

public:
    Q_INVOKABLE OffensiveHorse(Suit suit, int number);

    int extraOutDistance() const;

protected:
    int m_extraOutDistance;
};

class DefensiveHorse : public Horse
{
    Q_OBJECT

public:
    Q_INVOKABLE DefensiveHorse(Suit suit, int number);

    int extraInDistance() const;

protected:
    int m_extraInDistance;
};

class Treasure : public EquipCard
{
    Q_OBJECT

public:
    Treasure(Suit suit, int number);
};

#endif // CARD_H
