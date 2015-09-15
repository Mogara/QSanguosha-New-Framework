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
#include "player.h"
#include "structs.h"

CardsMoveStruct::Area::Area()
    : type(CardArea::Unknown)
    , direction(CardArea::UndefinedDirection)
    , owner(NULL)
{
}

QVariant CardsMoveStruct::Area::toVariant() const
{
    QVariantMap data;
    data["type"] = type;
    data["direction"] = direction;
    data["ownerId"] = owner ? owner->id() : 0;
    data["name"] = name;
    return data;
}

CardsMoveStruct::CardsMoveStruct()
    : isOpen(false)
    , isLastHandCard(false)
    , origin(NULL)
{
}

CardsMoveStruct::~CardsMoveStruct()
{
    if (origin)
        delete origin;
}

bool CardsMoveStruct::isRelevant(const Player *player) const
{
    return player == NULL || player == from.owner || (player == to.owner && to.type != CardArea::Special);
}

QVariant CardsMoveStruct::toVariant(bool open) const
{
    QVariantMap data;
    data["from"] = from.toVariant();
    data["to"] = to.toVariant();

    if (isOpen || open) {
        QVariantList cardData;
        foreach (const Card *card, cards)
            cardData << card->id();
        data["cards"] = cardData;
    } else {
        data["cards"] = cards.length();
    }

    data["isOpen"] = isOpen;
    data["isLastHandCard"] = isLastHandCard;
    return data;
}

CardUseStruct::CardUseStruct()
    : from(NULL)
    , card(NULL)
    , isOwnerUse(true)
    , addHistory(true)
    , isHandcard(true)
    , reason(PlayReason)
{
}


CardEffectStruct::CardEffectStruct()
    : card(NULL)
    , from(NULL)
    , to(NULL)
    , multiple(false)
    , nullified(false)
{
}
