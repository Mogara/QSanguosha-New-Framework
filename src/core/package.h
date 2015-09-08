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

#ifndef PACKAGE_H
#define PACKAGE_H

#include <QString>
#include <QList>

class Card;
class General;

class Package
{
public:
    Package(const QString &name);
    virtual ~Package();

    QString name() const { return m_name; }
    QList<const General *> generals() const;
    QList<const Card *> cards() const;

protected:
    void addGeneral(General *general) { m_generals << general; }
    void addGenerals(const QList<General *> &generals) { m_generals << generals; }
    void addCard(Card *card) { m_cards << card; }
    void addCards(const QList<Card *> &cards) { m_cards << cards; }

    QString m_name;
    QList<General *> m_generals;
    QList<Card *> m_cards;
};

#endif // PACKAGE_H
