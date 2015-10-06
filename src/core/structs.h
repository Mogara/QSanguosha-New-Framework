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
#include "cardpattern.h"
#include "player.h"

#include <QList>
#include <QString>
#include <QVariant>

class Card;
class ServerPlayer;

struct CardsMoveStruct
{
    struct Area
    {
        CardArea::Type type;
        CardArea::Direction direction;
        Player *owner;
        QString name;

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

Q_DECLARE_METATYPE(QList<CardsMoveStruct> *)

struct PhaseChangeStruct
{
    Player::Phase from;
    Player::Phase to;
};

Q_DECLARE_METATYPE(PhaseChangeStruct *)

struct CardUseStruct
{
    enum Reason
    {
        UnknownReason = 0x00,
        PlayReason = 0x01,
        ResponseReason = 0x02,
        ResponseUseReason = 0x12
    };

    ServerPlayer *from;
    QList<ServerPlayer *> to;
    Card *card;
    Card *target;
    QList<ServerPlayer *> nullifiedList;
    bool isNullified;
    bool isOwnerUse;
    bool addHistory;
    bool isHandcard;
    Reason reason;
    QVariant extra;

    CardUseStruct();
};

Q_DECLARE_METATYPE(CardUseStruct *)

struct CardEffectStruct
{
    CardUseStruct &use;
    ServerPlayer *&from;
    ServerPlayer *to;

    CardEffectStruct(CardUseStruct &use);
    bool isNullified() const {
        return use.isNullified || (to && use.nullifiedList.contains(to));
    }
};

Q_DECLARE_METATYPE(CardEffectStruct *)

struct DamageStruct
{
    enum Nature
    {
        Normal,
        Fire,
        Thunder
    };

    ServerPlayer *from;
    ServerPlayer *to;
    const Card *card;
    int damage;
    Nature nature;
    bool chain;
    bool transfer;
    bool byUser;
    QString reason;
    QString transferReason;
    bool prevented;

    DamageStruct();
};

Q_DECLARE_METATYPE(DamageStruct *)

struct RecoverStruct
{
    ServerPlayer *from;
    ServerPlayer *to;
    int recover;
    Card *card;

    RecoverStruct();
};

Q_DECLARE_METATYPE(RecoverStruct *)

struct CardResponseStruct
{
    ServerPlayer *from;
    ServerPlayer *to;
    Card *card;
    Card *target;

    CardResponseStruct();
};

Q_DECLARE_METATYPE(CardResponseStruct *)

struct JudgeStruct
{
    ServerPlayer *who;
    Card *card;
    bool matched;

    JudgeStruct(const QString &pattern);
    void updateResult();

private:
    CardPattern m_pattern;
};

Q_DECLARE_METATYPE(JudgeStruct *)

#endif // STRUCTS_H
