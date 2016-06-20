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
#include <QtQml>

namespace DataValue
{
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, CardMove);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, CardsMoveValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, PhaseChangeValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, CardUseValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, CardEffectValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, DamageValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, RecoverValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, CardResponseValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, JudgeValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, DeathValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, SkillValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, SkillInvokeValue);
    C_REGISTER_QMLTYPE("Sanguosha.DataValues", 1, 0, IntValue);

    CardMove::CardMove()
        : fromArea(nullptr), toArea(nullptr), toDirection(CardArea::UndefinedDirection), card(nullptr), isOpen(false)
    {

    }


    CardMove::CardMove(const CardMove &move)
        : fromArea(move.fromArea), toArea(move.toArea), toDirection(move.toDirection), card(move.card), isOpen(move.isOpen)
    {

    }

    bool CardMove::operator ==(const CardMove &move)
    {
        return fromArea == move.fromArea && toArea == move.toArea && toDirection == move.toDirection && card == move.card && isOpen == move.isOpen;
    }

    CardMove &CardMove::operator=(const CardMove &move)
    {
        fromArea = move.fromArea;
        toArea = move.toArea;
        toDirection = move.toDirection;
        card = move.card;
        isOpen = move.isOpen;

        return *this;
    }

    bool CardMove::isRelevant(const Player *player) const
    {
        if (player == nullptr)
            return true;

        if (fromArea != nullptr) {
            if (fromArea->type() != CardArea::Special && player == fromArea->owner())
                return true;
        }

        if (toArea != nullptr) {
            if (toArea->type() != CardArea::Special && player == toArea->owner())
                return true;
        }

        return false;
    }

    QVariant CardMove::toVariant(bool open /*= false*/) const
    {
        QVariantMap map;
        if (fromArea != nullptr)
            map["fromArea"] = fromArea->toVariant();
        if (toArea != nullptr)
            map["toArea"] = toArea->toVariant();
        map["toDirection"] = static_cast<int>(toDirection);
        if (card != nullptr && (isOpen || open))
            map["card"] = card->id();
        map["isOpen"] = isOpen;
        return map;
    }

    CardsMoveValue::CardsMoveValue()
        : movesProp(this, moves)
        , virtualMovesProp(this, virtualMoves)
    {

    }


    CardsMoveValue::CardsMoveValue(const CardsMoveValue &move)
        : moves(move.moves)
        , movesProp(this, moves)
        , virtualMovesProp(this, virtualMoves)
    {

    }

    QVariant CardsMoveValue::toVariant(bool open /*= false*/) const
    {
        QVariantMap map;
        QVariantList list;
        foreach(CardMove *move, moves)
            list << move->toVariant(open);
        map["moves"] = list;
        return map;
    }

    QVariant CardsMoveValue::toVariant(const Player *relevantPlayer) const
    {
        QVariantMap map;
        QVariantList list;
        foreach(const CardMove *move, moves)
            list << move->toVariant(move->isRelevant(relevantPlayer));
        map["moves"] = list;
        return map;
    }

    CardsMoveValue &CardsMoveValue::operator=(const CardsMoveValue &move)
    {
        moves = move.moves;
        return *this;
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
        , toProp(this, to)
        , nullifiedListProp(this, nullifiedList)
    {
    }

    CardUseValue::CardUseValue(const CardUseValue &arg2)
        : QObject()
        , toProp(this, to)
        , nullifiedListProp(this, nullifiedList)
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

    CardEffectValue::CardEffectValue(const CardUseValue &use)
        : use(&use)
        , from(use.from)
        , to(nullptr)
    {
    }

    CardEffectValue::CardEffectValue()
        : use(nullptr), from(nullptr), to(nullptr)
    {

    }

    bool CardEffectValue::isNullified() const
    {
        if (use->isNullified)
            return true;

        for (int i = 0; i < use->nullifiedList.count(); ++i) {
            if (use->nullifiedList.at(i) == to)
                return true;
        }

        return false;
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

    JudgeValue::JudgeValue()
        : who(nullptr)
        , card(nullptr)
        , matched(false)
        , m_pattern(".")
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
        , targetsProp(this, targets)
        , cardsProp(this, cards)
    {

    }

    SkillInvokeValue::SkillInvokeValue(const SkillInvokeValue &arg2)
        : QObject()
        , targetsProp(this, targets)
        , cardsProp(this, cards)
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
}
