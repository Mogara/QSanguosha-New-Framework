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

#ifndef CARDAREA_H
#define CARDAREA_H

#include <QList>
#include <QVariant>
#include <QObject>
#include <functional>

#include <CardirectorGlobal>

class Player;
class Card;

class CardArea : public QObject
{
    Q_OBJECT

public:
    enum Type
    {
        Unknown,
        Hand, Equip, DelayedTrick, Judge,
        DrawPile, DiscardPile,
        Special, Table, Wugu
    };
    Q_ENUM(Type)

    enum Direction
    {
        UndefinedDirection = 0,
        Top = 0x1,
        Bottom = 0x2
    };
    Q_ENUM(Direction)

    typedef std::function<void()> ChangeSignal;

    Q_INVOKABLE explicit CardArea(Type type = Unknown, Player *owner = nullptr, const QString &name = QString());
    Q_INVOKABLE Type type() const { return m_type; }
    Q_INVOKABLE void setType(Type type) { m_type = type; }
    Q_INVOKABLE Player *owner() const { return m_owner; }
    Q_INVOKABLE void setOwner(Player *owner) { m_owner = owner; }
    Q_INVOKABLE QString name() const { return m_name; }
    Q_INVOKABLE void setName(const QString &name) { m_name = name; }

    void setSignal(ChangeSignal signal) { m_changeSignal = signal; }

    Q_INVOKABLE bool isVirtualCardArea() const { return m_isVirtualCardArea; }
    Q_INVOKABLE void setIsVirtualCardArea(bool keep) { m_isVirtualCardArea = keep; }

    Q_INVOKABLE bool add(Card *card, Direction direction = UndefinedDirection);
    Q_INVOKABLE bool add(const QList<Card *> &cards, Direction direction = UndefinedDirection);
    Q_INVOKABLE bool remove(Card *card);
    Q_INVOKABLE bool remove(const QList<Card *> &cards);
    Q_INVOKABLE void clear();

    // Note: These functions can only find Real Cards!!!!
    Q_INVOKABLE Card *findCard(uint id) const;
    Q_INVOKABLE Card *rand() const;
    Q_INVOKABLE QList<Card *> &cards() { return m_cards; }
    Q_INVOKABLE const QList<Card *> &cards() const { return m_cards; }
    Q_INVOKABLE void shuffleCards();

    // Note: These functions can only find Virtual Cards!!!!
    Q_INVOKABLE QList<Card *> &virtualCards() { return m_virtualCards; }
    Q_INVOKABLE const QList<Card *> &virtualCards() const { return m_virtualCards; }

    // Fs: make these functions get virtual cards by default in virtual card area, add a parameter like "forceRealCard" and set a default value of false
    Q_INVOKABLE Card *first(bool forceRealCard = false) const;
    Q_INVOKABLE Card *takeFirst(bool forceRealCard = false);

    Q_INVOKABLE Card *last(bool forceRealCard = false) const;
    Q_INVOKABLE Card *takeLast(bool forceRealCard = false);

    Q_INVOKABLE QList<Card *> first(int n, bool forceRealCard = false) const;
    Q_INVOKABLE QList<Card *> takeFirst(int n, bool forceRealCard = false);

    Q_INVOKABLE QList<Card *> last(int n, bool forceRealCard = false) const;
    Q_INVOKABLE QList<Card *> takeLast(int n, bool forceRealCard = false);

    // Fs: judge virtual cards by default in virtual card area, add a parameter like "forceRealCard" and set a default value of false
    Q_INVOKABLE bool contains(const Card *card, bool forceRealCard = false) const;
    Q_INVOKABLE bool contains(uint id, bool forceRealCard = false) const;
    Q_INVOKABLE bool contains(const char *className, bool forceRealCard = false) const;

    Q_INVOKABLE int length(bool forceRealCard = false) const;

    QVariant toVariant() const;

private:
    bool addCardProcedure(Card *card, Direction direction);
    bool removeCardProcedure(Card *card);

private:
    Type m_type;
    Player *m_owner;
    QString m_name;
    QList<Card *> m_cards;
    ChangeSignal m_changeSignal;
    bool m_isVirtualCardArea;
    QList<Card *> m_virtualCards; // Should we use QSharedPointer to deal with virtual cards?

private:
    C_DISABLE_COPY(CardArea);
};

Q_DECLARE_METATYPE(CardArea::Type)
Q_DECLARE_METATYPE(CardArea::Direction)


#endif // CARDAREA_H
