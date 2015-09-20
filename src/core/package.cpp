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

#include "package.h"
#include "card.h"
#include "general.h"

Package::Package(const QString &name)
    : m_name(name)
{
}

Package::~Package()
{
    foreach (Card *card, m_cards)
        delete card;

    foreach (General *general, m_generals)
        delete general;
}

QList<const General *> Package::generals(bool includeHidden) const
{
    QList<const General *> generals;
    generals.reserve(m_generals.length());
    foreach (const General *general, m_generals) {
        if (includeHidden || !general->isHidden())
            generals << general;
    }
    return generals;
}

QList<const Card *> Package::cards() const
{
    QList<const Card *> cards;
    cards.reserve(m_cards.length());
    foreach (const Card *card, m_cards)
        cards << card;
    return cards;
}
