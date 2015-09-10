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

#include "player.h"

#include <QList>
#include <QString>
#include <QVariant>

class Card;
class Player;

struct CardsMoveStruct
{
    struct Place
    {
        Player::Area area;
        Player *owner;
        QString pile;

        Place();
        QVariant toVariant() const;
    };

    Place from;
    Place to;
    QList<const Card *> cards;
    bool isOpen;
    bool isLastHandCard;
    CardsMoveStruct *origin;

    CardsMoveStruct();
    ~CardsMoveStruct();

    bool isRelevant(const Player *player) const;
    QVariant toVariant(bool open = false) const;
};

#endif // STRUCTS_H
