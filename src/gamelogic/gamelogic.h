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

#ifndef CGAMELOGIC_H
#define CGAMELOGIC_H

#include "event.h"
#include "eventtype.h"
#include "structs.h"

#include <cabstractgamelogic.h>

class Card;
class GameRule;
class ServerPlayer;
class Package;

class GameLogic : public CAbstractGameLogic
{
    Q_OBJECT

public:
    GameLogic(CRoom *parent = 0);
    ~GameLogic();

    void setGameRule(const GameRule *rule);
    void setPackages(const QList<const Package *> &packages) { m_packages = packages; }

    void addEventHandler(const EventHandler *handler);
    bool trigger(EventType event, ServerPlayer *target);
    bool trigger(EventType event, ServerPlayer *target, QVariant &data);

    void setCurrentPlayer(ServerPlayer *player) { m_currentPlayer = player; }
    ServerPlayer *currentPlayer() const { return m_currentPlayer; }

    QList<ServerPlayer *> players() const;
    ServerPlayer *findPlayer(uint id) const;
    ServerPlayer *findPlayer(CServerAgent *agent) const;

    QList<ServerPlayer *> allPlayers(bool includeDead = false) const;

    void addExtraTurn(ServerPlayer *player) { m_extraTurns << player; }
    QList<ServerPlayer *> extraTurns() const { return m_extraTurns; }

    bool isGlobalRequestEnabled() const { return m_globalRequestEnabled; }
    bool skipGameRule() const { return m_skipGameRule; }

    void moveCards(const CardsMoveStruct &move);
    void moveCards(const QList<CardsMoveStruct> &moves);

protected:
    CAbstractPlayer *createPlayer(CServerUser *user);
    CAbstractPlayer *createPlayer(CServerRobot *robot);

    void prepareToStart();

    void run();

private:
    QList<const EventHandler *> m_handlers[EventTypeCount];
    QList<ServerPlayer *> m_players;
    ServerPlayer *m_currentPlayer;
    QList<ServerPlayer *> m_extraTurns;
    const GameRule *m_gameRule;
    QList<const Package *> m_packages;
    QList<Card *> m_cards;
    bool m_globalRequestEnabled;
    bool m_skipGameRule;
    int m_round;
};

#endif // CGAMELOGIC_H
