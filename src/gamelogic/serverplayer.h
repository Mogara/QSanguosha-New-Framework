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

#ifndef SERVERPLAYER_H
#define SERVERPLAYER_H

#include <QPointer>

#include "event.h"
#include "player.h"
#include "structs.h"

class CRoom;
class CServerAgent;
class GameLogic;

class ServerPlayer : public Player
{
    Q_OBJECT

public:
    ServerPlayer(GameLogic *logic, CServerAgent *agent);
    ~ServerPlayer();

    CServerAgent *agent() const;
    void setAgent(CServerAgent *agent);

    CRoom *room() const;

    ServerPlayer *next() const { return qobject_cast<ServerPlayer *>(Player::next()); }
    ServerPlayer *next(bool ignoreRemoved) const{ return qobject_cast<ServerPlayer *>(Player::next(ignoreRemoved)); }
    ServerPlayer *nextAlive(int step = 1, bool ignoreRemoved = true) const{ return qobject_cast<ServerPlayer *>(Player::nextAlive(step, ignoreRemoved)); }

    void drawCards(int n);
    void play();
    void play(const QList<Phase> &phases);
    void activate(CardUseStruct &use);

    Event askForTriggerOrder(const QString &reason, QList<Event> &options, bool cancelable);
    void broadcastProperty(const char *name) const;

    void addCardHistory(const QString &name, int times = 1);
    void clearCardHistory();

private:
    GameLogic *m_logic;
    CRoom *m_room;
    QPointer<CServerAgent> m_agent;
    CardArea *m_handcards;
};

#endif // SERVERPLAYER_H
