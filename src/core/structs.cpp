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
#include "serverplayer.h"
#include "structs.h"

CardsMoveStruct::Area::Area()
    : type(CardArea::Unknown)
    , direction(CardArea::UndefinedDirection)
    , owner(nullptr)
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
    , origin(nullptr)
{
}

CardsMoveStruct::~CardsMoveStruct()
{
    if (origin)
        delete origin;
}

bool CardsMoveStruct::isRelevant(const Player *player) const
{
    return player == nullptr || player == from.owner || (player == to.owner && to.type != CardArea::Special);
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
    : from(nullptr)
    , card(nullptr)
    , target(nullptr)
    , isNullified(false)
    , isOwnerUse(true)
    , addHistory(true)
    , isHandcard(true)
    , reason(PlayReason)
{
}

CardEffectStruct::CardEffectStruct(CardUseStruct &use)
    : use(use)
    , from(use.from)
    , to(nullptr)
{
}

DamageStruct::DamageStruct()
    : from(nullptr)
    , to(nullptr)
    , card(nullptr)
    , damage(1)
    , nature(Normal)
    , chain(false)
    , transfer(false)
    , byUser(true)
    , prevented(false)
{
}

RecoverStruct::RecoverStruct()
    : from(nullptr)
    , to(nullptr)
    , recover(1)
    , card(nullptr)
{
}

CardResponseStruct::CardResponseStruct()
    : from(nullptr)
    , to(nullptr)
    , card(nullptr)
    , target(nullptr)
{
}

JudgeStruct::JudgeStruct(const QString &pattern)
    : who(nullptr)
    , card(nullptr)
    , matched(false)
    , m_pattern(pattern)
{
}

void JudgeStruct::updateResult()
{
    matched = m_pattern.match(who, card);
}

DeathStruct::DeathStruct()
    : who(nullptr)
    , damage(nullptr)
{
}
