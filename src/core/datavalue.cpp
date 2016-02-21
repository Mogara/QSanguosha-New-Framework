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
#include "serverplayer.h"
#include "datavalue.h"

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

PhaseChangeValue::PhaseChangeValue() 
    : from(Player::InvalidPhase), to(Player::InvalidPhase)
{

}

CardUseValue::CardUseValue()
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

CardUseValue::CardUseValue(const CardUseValue &arg2)
    : QObject()
{
    from = arg2.from;
    to = arg2.to;
    card = arg2.card;
    target = arg2.target;
    nullifiedList = arg2.nullifiedList;
    isNullified = arg2.isNullified;
    isOwnerUse = arg2.isOwnerUse;
    addHistory = arg2.addHistory;
    isHandcard = arg2.isHandcard;
    reason = arg2.reason;
    extra = arg2.extra;
}

CardUseValue &CardUseValue::operator =(const CardUseValue &arg2)
{
    from = arg2.from;
    to = arg2.to;
    card = arg2.card;
    target = arg2.target;
    nullifiedList = arg2.nullifiedList;
    isNullified = arg2.isNullified;
    isOwnerUse = arg2.isOwnerUse;
    addHistory = arg2.addHistory;
    isHandcard = arg2.isHandcard;
    reason = arg2.reason;
    extra = arg2.extra;

    return *this;
}

CardEffectValue::CardEffectValue(CardUseValue &use)
    : use(use)
    , from(use.from)
    , to(nullptr)
{
}

DamageValue::DamageValue()
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

RecoverValue::RecoverValue()
    : from(nullptr)
    , to(nullptr)
    , recover(1)
    , card(nullptr)
{
}

CardResponseValue::CardResponseValue()
    : from(nullptr)
    , to(nullptr)
    , card(nullptr)
    , target(nullptr)
    , isRetrial(false)
{
}

JudgeValue::JudgeValue(const QString &pattern)
    : who(nullptr)
    , card(nullptr)
    , matched(false)
    , m_pattern(pattern)
{
}

void JudgeValue::updateResult()
{
    matched = m_pattern.match(who, card);
}

DeathValue::DeathValue()
    : who(nullptr)
    , damage(nullptr)
{
}

SkillValue::SkillValue()
    : owner(nullptr)
    , skill(nullptr)
    , area(Player::UnknownSkillArea)
{
}

SkillInvokeValue::SkillInvokeValue()
    : player(nullptr)
    , skill(nullptr)
{

}

SkillInvokeValue::SkillInvokeValue(const SkillInvokeValue &arg2)
    : QObject()
{
    player = arg2.player;
    skill = arg2.skill;
    targets = arg2.targets;
    cards = arg2.cards;
}

SkillInvokeValue &SkillInvokeValue::operator =(const SkillInvokeValue &arg2)
{
    player = arg2.player;
    skill = arg2.skill;
    targets = arg2.targets;
    cards = arg2.cards;

    return *this;
}

IntValue::IntValue(int value)
    : value(value)
{

}
