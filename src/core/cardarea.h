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
#include <functional>

class Player;
class Card;

class CardArea
{
public:
    enum Type
    {
        Unknown,
        Hand, Equip, DelayedTrick, Judge,
        DrawPile, DiscardPile,
        Special, Table, Wugu
    };

    enum Direction
    {
        UndefinedDirection,
        Top,
        Bottom
    };

    typedef std::function<void()> ChangeSignal;

    CardArea(Type type, Player *owner = nullptr, const QString &name = QString());
    Type type() const { return m_type; }
    Player *owner() const { return m_owner; }
    QString name() const { return m_name; }

    void setSignal(ChangeSignal signal) { m_changeSignal = signal; }

    bool add(Card *card, Direction direction = UndefinedDirection);
    bool add(const QList<Card *> &cards, Direction direction = UndefinedDirection);
    bool remove(Card *card);
    bool remove(const QList<Card *> &cards);
    void clear() { m_cards.clear(); }

    Card *findCard(uint id) const;
    Card *rand() const;

    Card *first() const { return m_cards.first(); }
    Card *takeFirst() { return m_cards.takeFirst(); }

    Card *last() const { return m_cards.last(); }
    Card *takeLast() { return m_cards.takeLast(); }

    QList<Card *> first(int n) const { return m_cards.mid(0, n); }
    QList<Card *> takeFirst(int n);

    QList<Card *> last(int n) const { return m_cards.mid(m_cards.length() - n); }
    QList<Card *> takeLast(int n);

    bool contains(const Card *card) const;
    bool contains(uint id) const;

    QList<Card *> &cards() { return m_cards; }
    QList<Card *> cards() const { return m_cards; }

    int length() const { return m_cards.length(); }
    int size() const { return m_cards.size(); }

private:
    Type m_type;
    Player *m_owner;
    QString m_name;
    QList<Card *> m_cards;
    ChangeSignal m_changeSignal;
};

#endif // CARDAREA_H
