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
        DrawPile, DrawPileTop, DrawPileBottom, DiscardPile,
        Special, Table, Wugu
    };

    typedef std::function<void()> ChangeSignal;

    CardArea(Type type, Player *owner = NULL);
    Type type() const { return m_type; }
    Player *owner() const { return m_owner; }

    void setSignal(ChangeSignal signal) { m_changeSignal = signal; }

    bool add(Card *card);
    bool add(const QList<Card *> &cards);
    bool remove(Card *card);
    bool remove(const QList<Card *> &cards);

    bool contains(Card *card) const { return m_cards.contains(card); }
    QList<Card *> &cards() { return m_cards; }
    QList<const Card *> cards() const;
    int length() const { return m_cards.length(); }
    int size() const { return m_cards.size(); }

private:
    Type m_type;
    Player *m_owner;
    QList<Card *> m_cards;
    ChangeSignal m_changeSignal;
};

#endif // CARDAREA_H
