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

#ifndef GAMELOGGER_H
#define GAMELOGGER_H

#include <QObject>

#include "structs.h"

class Client;
class ClientPlayer;

class GameLogger : public QObject
{
    Q_OBJECT

public:
    GameLogger(Client *client, QObject *parent = nullptr);

signals:
    void logAdded(const QString &log);

protected:
    void onSeatArranged();
    void onDamageDone(const ClientPlayer *from, const ClientPlayer *to, DamageStruct::Nature nature, int damage);
    void onLoseHpDone(const ClientPlayer *victim, int lose);
    void onRecoverDone(const ClientPlayer *from, const ClientPlayer *to, int num);
    void onCardUsed(const QVariantMap &card, const ClientPlayer *from, const QList<const ClientPlayer *> &tos);
    void onCardShown(const ClientPlayer *from, const QList<const Card *> &cards);
    void onSkillInvoked(const ClientPlayer *invoker, const Skill *skill, const QList<const Card *> &cards, const QList<const ClientPlayer *> &targets);
};

#endif // GAMELOGGER_H
