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

#include "cardarea.h"

CardArea::CardArea(CardArea::Type type, Player *owner, const QString &name)
    : m_type(type)
    , m_owner(owner)
    , m_name(name)
{
}

bool CardArea::add(Card *card, Direction direction) {
    if (m_cards.contains(card))
        return false;
    if (direction == Bottom)
        m_cards.prepend(card);
    else
        m_cards.append(card);
    if (m_changeSignal)
        m_changeSignal();
    return true;
}

bool CardArea::add(const QList<Card *> &cards, Direction direction)
{
    int num = length();
    foreach (Card *card, cards) {
        if (m_cards.contains(card))
            continue;
        if (direction == Top)
            m_cards.prepend(card);
        else
            m_cards.append(card);
    }

    if (m_changeSignal && num != length())
            m_changeSignal();

    return num + cards.length() == length();
}

bool CardArea::remove(Card *card)
{
    if (m_cards.removeOne(card)) {
        if (m_changeSignal)
            m_changeSignal();
        return true;
    }

    return false;
}

bool CardArea::remove(const QList<Card *> &cards)
{
    int num = length();
    foreach (Card *card, cards)
        m_cards.removeOne(card);

    if (m_changeSignal && num != length())
            m_changeSignal();

    return num - cards.length() == length();
}

QList<Card *> CardArea::takeFirst(int n)
{
    QList<Card *> cards = m_cards.mid(0, n);
    m_cards = m_cards.mid(n + 1);
    return cards;
}

QList<Card *> CardArea::takeLast(int n)
{
    QList<Card *> cards = m_cards.mid(length() - n);
    m_cards = m_cards.mid(0, length() - n);
    return cards;
}

bool CardArea::contains(const Card *card) const
{
    foreach (const Card *c, m_cards)
        if (c == card)
            return true;
    return false;
}
