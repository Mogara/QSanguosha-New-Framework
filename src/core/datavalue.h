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
#include <QObject>

class Card;
class ServerPlayer;
class Skill;
class QJSEngine;

class CardsMoveStruct
{
public:
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

class CardsMoveValue : public QList<CardsMoveStruct>, public QObject
#pragma message ("warning: todo_Fs: the CardsMoveValue is temporary put here to make the codes running, should be re-defined")
{
public:
    CardsMoveValue(const QList<CardsMoveStruct> &arg2)
        : QList<CardsMoveStruct>(arg2), QObject()
    {
    }

    CardsMoveValue(const CardsMoveValue &arg2)
        : QList<CardsMoveStruct>(arg2), QObject()
    {
    }

    CardsMoveValue &operator =(const QList<CardsMoveStruct> &arg2)
    {
        QList<CardsMoveStruct>::operator =(arg2);
        return *this;
    }

    CardsMoveValue &operator =(CardsMoveValue &arg2)
    {
        QList<CardsMoveStruct>::operator =(arg2);
        return *this;
    }

private:
    CardsMoveValue() = delete;
};

class PhaseChangeValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Player::Phase from MEMBER from)
    Q_PROPERTY(Player::Phase to MEMBER to)

public:
    PhaseChangeValue();

    Player::Phase from;
    Player::Phase to;
};

class CardUseValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ServerPlayer *from MEMBER from)
    Q_PROPERTY(QList<ServerPlayer *> to MEMBER to)
    Q_PROPERTY(Card *card MEMBER card)
    Q_PROPERTY(Card *target MEMBER target)
    Q_PROPERTY(QList<ServerPlayer *> nullifiedList MEMBER nullifiedList)
    Q_PROPERTY(bool isNullified MEMBER isNullified)
    Q_PROPERTY(bool isOwnerUse MEMBER isOwnerUse)
    Q_PROPERTY(bool addHistory MEMBER addHistory)
    Q_PROPERTY(bool isHandcard MEMBER isHandcard)
    Q_PROPERTY(Reason reason MEMBER reason)
    Q_PROPERTY(QVariant extra MEMBER extra)

public:
    enum Reason
    {
        UnknownReason = 0x00,
        PlayReason = 0x01,
        ResponseReason = 0x02,
        ResponseUseReason = 0x12
    };
    Q_ENUM(Reason)

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

    CardUseValue();
    CardUseValue(const CardUseValue &arg2);
    CardUseValue &operator =(const CardUseValue &arg2);
};

class CardEffectValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CardUseValue use MEMBER use CONSTANT)
    Q_PROPERTY(ServerPlayer *from MEMBER from CONSTANT)
    Q_PROPERTY(ServerPlayer *to MEMBER to)

public:
    CardUseValue &use;
    ServerPlayer * &from;
    ServerPlayer *to;

    CardEffectValue(CardUseValue &use);

    Q_INVOKABLE bool isNullified() const { return use.isNullified || (to && use.nullifiedList.contains(to)); }
};

class DamageValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ServerPlayer *from MEMBER from)
    Q_PROPERTY(ServerPlayer *to MEMBER to)
    Q_PROPERTY(Card *card MEMBER card)
    Q_PROPERTY(int damage MEMBER damage)
    Q_PROPERTY(Nature natrue MEMBER nature)
    Q_PROPERTY(bool chain MEMBER chain)
    Q_PROPERTY(bool transfer MEMBER transfer)
    Q_PROPERTY(bool byUser MEMBER byUser)
    Q_PROPERTY(QString reason MEMBER reason)
    Q_PROPERTY(QString transferReason MEMBER transferReason)
    Q_PROPERTY(bool prevented MEMBER prevented)

public:
    enum Nature
    {
        Normal,
        Fire,
        Thunder
    };
    Q_ENUM(Nature)

    ServerPlayer *from;
    ServerPlayer *to;
    Card *card;
    int damage;
    Nature nature;
    bool chain;
    bool transfer;
    bool byUser;
    QString reason;
    QString transferReason;
    bool prevented;

    DamageValue();
};

class RecoverValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ServerPlayer *from MEMBER from)
    Q_PROPERTY(ServerPlayer *to MEMBER to)
    Q_PROPERTY(int recover MEMBER recover)
    Q_PROPERTY(Card *card MEMBER card)

public:
    ServerPlayer *from;
    ServerPlayer *to;
    int recover;
    Card *card;

    RecoverValue();
};

class CardResponseValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ServerPlayer *from MEMBER from)
    Q_PROPERTY(ServerPlayer *to MEMBER to)
    Q_PROPERTY(Card *card MEMBER card)
    Q_PROPERTY(Card *target MEMBER target)
    Q_PROPERTY(bool isRetrial MEMBER isRetrial)

public:
    ServerPlayer *from;
    ServerPlayer *to;
    Card *card;
    Card *target;

    bool isRetrial;

    CardResponseValue();
};

class JudgeValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ServerPlayer *who MEMBER who)
    Q_PROPERTY(Card *card MEMBER card)
    Q_PROPERTY(bool matched MEMBER matched)

public:
    ServerPlayer *who;
    Card *card;
    bool matched;

    JudgeValue(const QString &pattern);
    Q_INVOKABLE void updateResult();

private:
    CardPattern m_pattern;
};

class DeathValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ServerPlayer *who MEMBER who)
    Q_PROPERTY(DamageValue *damage MEMBER damage)

public:
    ServerPlayer *who;
    DamageValue *damage;

    DeathValue();
};

class SkillValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ServerPlayer *owner MEMBER owner)
    Q_PROPERTY(const Skill *skill MEMBER skill)
    Q_PROPERTY(Player::SkillArea area MEMBER area)

public:
    ServerPlayer *owner;
    const Skill *skill;
    Player::SkillArea area;

    SkillValue();
};

class SkillInvokeValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ServerPlayer *player MEMBER player)
    Q_PROPERTY(const Skill *skill MEMBER skill)
    Q_PROPERTY(QList<ServerPlayer *> targets MEMBER targets)
    Q_PROPERTY(QList<Card *> cards MEMBER cards)

public:
    ServerPlayer *player;
    const Skill *skill;
    QList<ServerPlayer *> targets;
    QList<Card *> cards;

    SkillInvokeValue();
    SkillInvokeValue(const SkillInvokeValue &arg2);
    SkillInvokeValue &operator =(const SkillInvokeValue &arg2);
};

class IntValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value MEMBER value)
        
public:
    int value;

    IntValue(int value);
};

#endif // STRUCTS_H
