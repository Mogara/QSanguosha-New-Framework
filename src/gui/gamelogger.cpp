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

#include "gamelogger.h"

#include "card.h"
#include "client.h"
#include "clientplayer.h"
#include "skill.h"

namespace LogStyle {
    QString From(const ClientPlayer *player)
    {
        return QString("<span style=\"color: #00FF00; font-weight:bold;\">%1</span>").arg(player->fullGeneralName());
    }

    QString To(const ClientPlayer *player)
    {
        return QString("<span style=\"color: #FF0000; font-weight:bold;\">%1</span>").arg(player->fullGeneralName());
    }

    QString To(const QList<const ClientPlayer *> &players)
    {
        QStringList strs;
        foreach (const ClientPlayer *player, players)
            strs << To(player);
        return strs.join(", ");
    }

    QString CardName(const QVariantMap &data)
    {
        return QString("<span style=\"color: yellow; font-weight: bold;\">%1[<img src=\"image://root/card/suit/%2.png\" />%3]</span>")
                .arg(QObject::tr(data["name"].toString().toLatin1().constData()))
                .arg(data["suit"].toString())
                .arg(data["number"].toString());
    }

    QString CardName(const Card *card)
    {
        return QString("<span style=\"color: yellow; font-weight: bold;\">%1[<img src=\"image://root/card/suit/%2.png\" />%3]</span>")
                .arg(QObject::tr(card->objectName().toLatin1().constData()))
                .arg(card->suitString())
                .arg(card->number());
    }

    QString CardName(const QList<const Card *> &cards)
    {
        QStringList strs;
        foreach (const Card *card, cards)
            strs << CardName(card);
        return strs.join(", ");
    }

    QString SkillName(const Skill *skill)
    {
        return QObject::tr("<span style=\"color: yellow; font-weight: bold;\">%1</span>")
                .arg(QObject::tr(skill->name().toLatin1().constData()));
    }

    inline QString Number(int number)
    {
        return QString("<span style=\"color: yellow; font-weight: bold;\">%1</span>").arg(number);
    }
}

GameLogger::GameLogger(Client *client, QObject *parent)
    : QObject(parent)
{
    connect(client, &Client::seatArranged, this, &GameLogger::onSeatArranged);
    connect(client, &Client::damageDone, this, &GameLogger::onDamageDone);
    connect(client, &Client::loseHpDone, this, &GameLogger::onLoseHpDone);
    connect(client, &Client::recoverDone, this, &GameLogger::onRecoverDone);
    connect(client, &Client::cardUsed, this, &GameLogger::onCardUsed);
    connect(client, &Client::cardShown, this, &GameLogger::onCardShown);
    connect(client, &Client::skillInvoked, this, &GameLogger::onSkillInvoked);
}

void GameLogger::onSeatArranged()
{
    Client *client = qobject_cast<Client *>(sender());
    QList<const ClientPlayer *> players = client->players();
    foreach (const ClientPlayer *player, players) {
        connect(player, &ClientPlayer::phaseChanged, [this,player](){
            if (player->phase() == Player::Start)
                emit logAdded("───────────────");
        });
    }
}

void GameLogger::onDamageDone(const ClientPlayer *from, const ClientPlayer *to, DamageValue::Nature nature, int damage)
{
    QString damageType = tr("damage");
    if (nature == DamageValue::Fire) {
        damageType = tr("fire damage");
    } else if (nature == DamageValue::Thunder) {
        damageType = tr("thunder damage");
    }

    if (from) {
        QString log = tr("%1 caused %3 points of %4 to %2")
                .arg(LogStyle::From(from))
                .arg(LogStyle::To(to))
                .arg(LogStyle::Number(damage))
                .arg(damageType);
        emit logAdded(log);
    } else {
        QString log = tr("%1 took %2 points of %3")
                .arg(LogStyle::To(to))
                .arg(LogStyle::Number(damage))
                .arg(damageType);
        emit logAdded(log);
    }
}

void GameLogger::onLoseHpDone(const ClientPlayer *victim, int lose)
{
    QString log = tr("%1 lost %2 points of hp")
            .arg(LogStyle::From(victim))
            .arg(LogStyle::Number(lose));
    emit logAdded(log);
}

void GameLogger::onRecoverDone(const ClientPlayer *from, const ClientPlayer *to, int num)
{
    QString log = tr("%1 recovered %2 %3 points of hp")
            .arg(LogStyle::From(from))
            .arg(LogStyle::To(to))
            .arg(LogStyle::Number(num));
    emit logAdded(log);
}

void GameLogger::onCardUsed(const QVariantMap &card, const ClientPlayer *from, const QList<const ClientPlayer *> &tos)
{
    QStringList toStr;
    foreach (const ClientPlayer *to, tos)
        toStr << LogStyle::To(to);

    QString log = tr("%1 used %2 upon %3")
            .arg(LogStyle::From(from))
            .arg(LogStyle::CardName(card))
            .arg(toStr.join(", "));
    emit logAdded(log);
}

void GameLogger::onCardShown(const ClientPlayer *from, const QList<const Card *> &cards)
{
    QString log = tr("%1 showed %2")
            .arg(LogStyle::From(from))
            .arg(LogStyle::CardName(cards));
    emit logAdded(log);
}

void GameLogger::onSkillInvoked(const ClientPlayer *invoker, const Skill *skill, const QList<const Card *> &cards, const QList<const ClientPlayer *> &targets)
{
    QString log = tr("%1 invoked %2")
            .arg(LogStyle::From(invoker))
            .arg(LogStyle::SkillName(skill));

    if (!cards.isEmpty())
        log.append(tr(", with cards %1").arg(LogStyle::CardName(cards)));

    if (!targets.isEmpty())
        log.append(tr(", with targets %2").arg(LogStyle::To(targets)));

    emit logAdded(log);
}
