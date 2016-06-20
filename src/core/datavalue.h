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
#include <QtQml>

class Card;
class ServerPlayer;
class Skill;

namespace DataValue
{
    class CardMove : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(CardArea *fromArea MEMBER fromArea)
        Q_PROPERTY(CardArea *toArea MEMBER toArea)
        Q_PROPERTY(CardArea::Direction toDirection MEMBER toDirection)
        Q_PROPERTY(Card *card MEMBER card)
        Q_PROPERTY(bool isOpen MEMBER isOpen)

    public:
        Q_INVOKABLE CardMove();
        CardMove(const CardMove &move);
        CardMove &operator =(const CardMove &move);
        bool operator ==(const CardMove &move);

        CardArea *fromArea;
        CardArea *toArea;
        CardArea::Direction toDirection;
        Card *card;
        bool isOpen;

        Q_INVOKABLE bool isRelevant(const Player *player) const;
        QVariant toVariant(bool open = false) const;
    };
    QML_DECLARE_TYPE(CardMove);

    class CardsMoveValue : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QQmlListProperty<CardMove> moves MEMBER movesProp CONSTANT)
        Q_PROPERTY(QQmlListProperty<CardMove> virtualMoves MEMBER virtualMovesProp CONSTANT)

    public:
        QList<CardMove *> moves;
        QList<CardMove *> virtualMoves;

        QQmlListProperty<CardMove> movesProp;
        QQmlListProperty<CardMove> virtualMovesProp;

        Q_INVOKABLE CardsMoveValue();
        CardsMoveValue(const CardsMoveValue &move);
        CardsMoveValue &operator =(const CardsMoveValue &move);

        QVariant toVariant(bool open = false) const;
        QVariant toVariant(const Player *relevantPlayer) const;
    };
    QML_DECLARE_TYPE(CardsMoveValue);

    class PhaseChangeValue : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(Player::Phase from MEMBER from)
        Q_PROPERTY(Player::Phase to MEMBER to)

    public:
        Q_INVOKABLE PhaseChangeValue();

        Player::Phase from;
        Player::Phase to;
    };
    QML_DECLARE_TYPE(PhaseChangeValue);

    class CardUseValue : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(ServerPlayer *from MEMBER from)
        Q_PROPERTY(QQmlListProperty<ServerPlayer> to MEMBER toProp CONSTANT)
        Q_PROPERTY(Card *card MEMBER card)
        Q_PROPERTY(Card *target MEMBER target)
        Q_PROPERTY(QQmlListProperty<ServerPlayer> nullifiedList MEMBER nullifiedListProp CONSTANT)
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

        QQmlListProperty<ServerPlayer> toProp;
        QQmlListProperty<ServerPlayer> nullifiedListProp;

        Q_INVOKABLE CardUseValue();
        CardUseValue(const CardUseValue &arg2);
        CardUseValue &operator =(const CardUseValue &arg2);
    };
    QML_DECLARE_TYPE(CardUseValue);
    Q_DECLARE_METATYPE(CardUseValue::Reason);

    class CardEffectValue : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(const CardUseValue *use MEMBER use)
        Q_PROPERTY(ServerPlayer *from MEMBER from)
        Q_PROPERTY(ServerPlayer *to MEMBER to)

    public:
        const CardUseValue *use;
        ServerPlayer *from;
        ServerPlayer *to;

        CardEffectValue(const CardUseValue &use);
        Q_INVOKABLE CardEffectValue();

        Q_INVOKABLE bool isNullified() const;
    };
    QML_DECLARE_TYPE(CardEffectValue);

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

        Q_INVOKABLE DamageValue();
    };
    QML_DECLARE_TYPE(DamageValue);
    Q_DECLARE_METATYPE(DamageValue::Nature);

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

        Q_INVOKABLE RecoverValue();
    };
    QML_DECLARE_TYPE(RecoverValue);

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

        Q_INVOKABLE CardResponseValue();
    };
    QML_DECLARE_TYPE(CardResponseValue);

    class JudgeValue : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(ServerPlayer *who MEMBER who)
        Q_PROPERTY(Card *card MEMBER card)
        Q_PROPERTY(bool matched MEMBER matched)
        Q_PROPERTY(QString pattern STORED false READ pattern WRITE setPattern)

    public:
        ServerPlayer *who;
        Card *card;
        bool matched;

        Q_INVOKABLE JudgeValue();
        JudgeValue(const QString &pattern);
        Q_INVOKABLE void updateResult();

        const QString &pattern() const { return m_pattern.toString(); }
        void setPattern(const QString &pattern) { m_pattern = CardPattern(pattern); }

    private:
        CardPattern m_pattern;
    };
    QML_DECLARE_TYPE(JudgeValue);

    class DeathValue : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(ServerPlayer *who MEMBER who)
        Q_PROPERTY(DamageValue *damage MEMBER damage)

    public:
        ServerPlayer *who;
        DamageValue *damage;

        Q_INVOKABLE DeathValue();
    };
    QML_DECLARE_TYPE(DeathValue);

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

        Q_INVOKABLE SkillValue();
    };
    QML_DECLARE_TYPE(SkillValue);

    class SkillInvokeValue : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(ServerPlayer *player MEMBER player)
        Q_PROPERTY(const Skill *skill MEMBER skill)
        Q_PROPERTY(QQmlListProperty<ServerPlayer> targets MEMBER targetsProp CONSTANT)
        Q_PROPERTY(QQmlListProperty<Card> cards MEMBER cardsProp CONSTANT)

    public:
        ServerPlayer *player;
        const Skill *skill;
        QList<ServerPlayer *> targets;
        QList<Card *> cards;

        QQmlListProperty<ServerPlayer> targetsProp;
        QQmlListProperty<Card> cardsProp;

        Q_INVOKABLE SkillInvokeValue();
        SkillInvokeValue(const SkillInvokeValue &arg2);
        SkillInvokeValue &operator =(const SkillInvokeValue &arg2);
    };
    QML_DECLARE_TYPE(SkillInvokeValue);

    class IntValue : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(int value MEMBER value)

    public:
        int value;

        Q_INVOKABLE IntValue(int value = 0);
    };
    QML_DECLARE_TYPE(IntValue);
}

#endif // STRUCTS_H
