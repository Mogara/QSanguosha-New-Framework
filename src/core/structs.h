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

#ifndef STRUCTS_H
#define STRUCTS_H

#include "cardarea.h"

#include <QList>
#include <QString>
#include <QVariant>

class Card;
class Player;

struct CardsMoveStruct
{
    struct Area
    {
        CardArea::Type type;
        CardArea::Direction direction;
        Player *owner;
        QString pile;

        Area();
        QVariant toVariant() const;
    };

    Area from;
    Area to;
    QList<Card *> cards;
    bool isOpen;
    bool isLastHandCard;
    CardsMoveStruct *origin;

    CardsMoveStruct();
    ~CardsMoveStruct();

    bool isRelevant(const Player *player) const;
    QVariant toVariant(bool open = false) const;
};

Q_DECLARE_METATYPE(QList<CardsMoveStruct>)

#endif // STRUCTS_H
