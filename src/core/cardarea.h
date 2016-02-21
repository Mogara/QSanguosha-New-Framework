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

#include <functional>

#include <CardirectorGlobal>

class Player;
class Card;

class CardArea
{
    Q_GADGET

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

    explicit CardArea(Type type, Player *owner = nullptr, const QString &name = QString());
    Q_INVOKABLE Type type() const { return m_type; }
    Q_INVOKABLE Player *owner() const { return m_owner; }
    Q_INVOKABLE QString name() const { return m_name; }

    void setSignal(ChangeSignal signal) { m_changeSignal = signal; }

    bool keepVirtualCard() const { return m_keepVirtualCard; }
    void setKeepVirtualCard(bool keep) { m_keepVirtualCard = keep; }

    bool add(Card *card, Direction direction = UndefinedDirection);
    bool add(const QList<Card *> &cards, Direction direction = UndefinedDirection);
    bool remove(Card *card);
    bool remove(const QList<Card *> &cards);
    void clear() { m_cards.clear(); }

    Q_INVOKABLE Card *findCard(uint id) const;
    Q_INVOKABLE Card *rand() const;

    Q_INVOKABLE Card *first() const { return m_cards.first(); }
    Q_INVOKABLE Card *takeFirst() { return m_cards.takeFirst(); }

    Q_INVOKABLE Card *last() const { return m_cards.last(); }
    Q_INVOKABLE Card *takeLast() { return m_cards.takeLast(); }

    Q_INVOKABLE QList<Card *> first(int n) const { return m_cards.mid(0, n); }
    Q_INVOKABLE QList<Card *> takeFirst(int n);

    Q_INVOKABLE QList<Card *> last(int n) const { return m_cards.mid(m_cards.length() - n); }
    Q_INVOKABLE QList<Card *> takeLast(int n);

    Q_INVOKABLE bool contains(const Card *card) const;
    Q_INVOKABLE bool contains(uint id) const;

    void addVirtualCard(const QString &name) { m_virtualCards << name; }
    void removeVirtualCard(const QString &name) { m_virtualCards.removeOne(name); }
    Q_INVOKABLE bool contains(const char *className) const;

    Q_INVOKABLE QList<Card *> &cards() { return m_cards; }
    Q_INVOKABLE QList<Card *> cards() const { return m_cards; }

    Q_INVOKABLE int length() const { return m_cards.length(); }
    Q_INVOKABLE int size() const { return m_cards.size(); }

    QVariant toVariant() const;

private:
    Type m_type;
    Player *m_owner;
    QString m_name;
    QList<Card *> m_cards;
    ChangeSignal m_changeSignal;
    bool m_keepVirtualCard;
    QStringList m_virtualCards;

private:
    C_DISABLE_COPY(CardArea);
};

#endif // CARDAREA_H
