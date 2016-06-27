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

#include "card.h"
#include "cardarea.h"
#include "util.h"

#include <QtQml>

C_REGISTER_QMLTYPE("Sanguosha.CardArea", 1, 0, CardArea);


CardArea::CardArea(CardArea::Type type, Player *owner, const QString &name)
    : m_type(type)
    , m_owner(owner)
    , m_name(name)
    , m_isVirtualCardArea(false)
{
}

bool CardArea::add(Card *card, Direction direction)
{
    bool r = addCardProcedure(card, direction);
    if (r && m_changeSignal)
        m_changeSignal();

    return r;
}

bool CardArea::add(const QList<Card *> &cards, Direction direction)
{
    bool returnFlag = true;
    bool signalFlag = false;
    if (direction == Bottom) {
        for (int i = 0; i < cards.length(); ++i) {
            Card *card = cards.value(i);
            if (!addCardProcedure(card, direction))
                returnFlag = false;
            else
                signalFlag = true;
        }
    } else {
        for (int i = cards.length() - 1; i >= 0; --i) {
            Card *card = cards.value(i);
            if (!addCardProcedure(card, direction))
                returnFlag = false;
            else
                signalFlag = true;
        }
    }

    if (signalFlag && m_changeSignal)
        m_changeSignal();

    return returnFlag;
}

bool CardArea::remove(Card *card)
{
    if (removeCardProcedure(card)) {
        if (m_changeSignal)
            m_changeSignal();
        return true;
    }

    return false;
}

bool CardArea::remove(const QList<Card *> &cards)
{
    bool returnFlag = true;
    bool signalFlag = false;
    foreach (Card *card, cards) {
        if (removeCardProcedure(card))
            signalFlag = true;
        else
            returnFlag = false;
    }

    if (m_changeSignal && signalFlag)
        m_changeSignal();

    return returnFlag;
}

void CardArea::clear()
{
    m_cards.clear(); m_virtualCards.clear();
}

Card *CardArea::findCard(uint id) const
{
    foreach (Card *card, m_cards) {
        if (card && card->id() == id)
            return card;
    }
    return nullptr;
}

Card *CardArea::rand() const
{
    if (m_cards.isEmpty())
        return nullptr;

    int index = qrand() % m_cards.length();
    return m_cards.at(index);
}

void CardArea::shuffleCards()
{
    qShuffle(m_cards);
}

QList<Card *> CardArea::takeFirst(int n, bool forceRealCard/* = false*/)
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    QList<Card *> cards = cardsToOperate.mid(0, n);
    remove(cards);
    return cards;
}

Card *CardArea::takeFirst(bool forceRealCard/* = false*/)
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    Card *card = cardsToOperate.first();
    remove(card);
    return card;
}

Card *CardArea::last(bool forceRealCard/* = false*/) const
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    return cardsToOperate.last();
}

QList<Card *> CardArea::last(int n, bool forceRealCard/* = false*/) const
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    return cardsToOperate.mid(m_cards.length() - n);
}

QList<Card *> CardArea::takeLast(int n, bool forceRealCard/* = false*/)
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    QList<Card *> cards = cardsToOperate.mid(length() - n);
    remove(cards);
    return cards;
}

Card *CardArea::takeLast(bool forceRealCard/* = false*/)
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    Card *card = cardsToOperate.last();
    remove(card);
    return card;
}

Card *CardArea::first(bool forceRealCard/* = false*/) const
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    return cardsToOperate.first();
}

QList<Card *> CardArea::first(int n, bool forceRealCard/* = false*/) const
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    return cardsToOperate.mid(0, n);
}

bool CardArea::contains(const Card *card, bool forceRealCard/* = false*/) const
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    foreach (const Card *c, cardsToOperate)
        if (c == card)
            return true;
    return false;
}

bool CardArea::contains(uint id, bool forceRealCard/* = false*/) const
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    foreach (const Card *card, cardsToOperate)
        if (card && card->id() == id)
            return true;
    return false;
}

bool CardArea::contains(const char *className, bool forceRealCard/* = false*/) const
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    foreach (const Card *card, cardsToOperate)
        if (card && card->inherits(className))
            return true;
    return false;
}

int CardArea::length(bool forceRealCard /*= false*/) const
{
    QList<Card *> cardsToOperate = (isVirtualCardArea() && !forceRealCard) ? m_virtualCards : m_cards;
    return cardsToOperate.length();
}

QVariant CardArea::toVariant() const
{
    QVariantMap data;
    data["ownerId"] = m_owner ? m_owner->id() : 0;
    data["type"] = type();
    data["name"] = name();
    return data;
}

bool CardArea::addCardProcedure(Card *card, Direction direction)
{
    if (card == nullptr)
        return false;

    if (card->isVirtual() && !m_isVirtualCardArea) // Fs: don't delete the pointer since we use deleteLater to virtual cards
        return false;

    QList<Card *> &cards = card->isVirtual() ? m_virtualCards : m_cards;
    if (direction == Bottom)
        cards.append(card);
    else
        cards.prepend(card);

    return true;
}

bool CardArea::removeCardProcedure(Card *card)
{
    if (card == nullptr)
        return false;

    QList<Card *> &cards = card->isVirtual() ? m_virtualCards : m_cards;
    return cards.removeOne(card);
}
