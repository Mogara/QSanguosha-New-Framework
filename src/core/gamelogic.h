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

#include <cabstractgamelogic.h>

class Player;

class GameLogic : public CAbstractGameLogic
{
public:
    GameLogic(CRoom *parent = 0);

    void setGameRule(const EventHandler *rule) { m_gameRule = rule; }

    void addEventHandler(const EventHandler *handler);
    bool trigger(EventType event, Player *target);
    bool trigger(EventType event, Player *target, QVariant &data);

    void setCurrentPlayer(Player *player) { m_currentPlayer = player; }
    Player *currentPlayer() const { return m_currentPlayer; }

    QList<Player *> players() const;
    Player *findPlayer(uint id) const;

    QList<Player *> allPlayers(bool include_dead = false) const;

    void addExtraTurn(Player *player) { m_extraTurns << player; }
    QList<Player *> extraTurns() const { return m_extraTurns; }

    bool isGlobalRequestEnabled() const { return m_globalRequestEnabled; }

    Event askForTriggerOrder(Player *player, const QString &reason, QList<Event> &options, bool cancelable = true);

protected:
    CAbstractPlayer *createPlayer(CServerUser *user);
    CAbstractPlayer *createPlayer(CServerRobot *robot);

    void run();

private:
    QList<const EventHandler *> m_handlers[EventTypeCount];
    QList<Player *> m_players;
    Player *m_currentPlayer;
    QList<Player *> m_extraTurns;
    const EventHandler *m_gameRule;
    bool m_globalRequestEnabled;
};

#endif // CGAMELOGIC_H
