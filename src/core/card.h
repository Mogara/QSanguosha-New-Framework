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
    Q_PROPERTY(QString skillName READ skillName WRITE setSkillName)

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
        SkillType,
        BasicType,
        TrickType,
        EquipType
    };

    Q_INVOKABLE Card(Suit suit = NoSuit, int number = 0);
    Card *clone() const;

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
    QString typeString() const;

    void addSubcard(Card *card);
    QList<Card *> subcards() const { return m_subcards; }

    Card *realCard();
    QList<Card *> realCards();

    void setTransferable(bool transferable) { m_transferable = transferable; }
    bool isTransferable() const { return m_transferable; }

    void setSkillName(const QString &name) { m_skillName = name; }
    QString skillName() const { return m_skillName; }

    void addFlag(const QString &flag) { m_flags.insert(flag); }
    void removeFlag(const QString &flag) { m_flags.remove(flag); }
    bool hasFlag(const QString &flag) const { return m_flags.contains(flag); }
    void clearFlags() { m_flags.clear(); }

    bool willThrow() const { return m_willThrow; }
    bool canRecast() const { return m_canRecast; }

    bool isTargetFixed() const { return m_targetFixed; }
    virtual bool targetFeasible(const QList<const Player *> &targets, const Player *self) const;
    virtual bool targetFilter(const QList<const Player *> targets, const Player *toSelect, const Player *self) const;
    virtual bool isAvailable(const Player *player) const;

    virtual void onUse(GameLogic *logic, CardUseStruct &use) const;

protected:
    uint m_id;
    Suit m_suit;
    int m_number;
    Color m_color;
    Type m_type;
    bool m_transferable;

    bool m_willThrow;
    bool m_canRecast;
    bool m_targetFixed;

    QString m_skillName;
    QList<Card *> m_subcards;
    QSet<QString> m_flags;
};

#endif // CARD_H
