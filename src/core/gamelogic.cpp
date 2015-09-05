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

#include "eventhandler.h"
#include "gamelogic.h"
#include "player.h"

#include <croom.h>
#include <cserver.h>
#include <QCoreApplication>

#include <QDateTime>

GameLogic::GameLogic(CRoom *parent)
    : CAbstractGameLogic(parent)
    , m_currentPlayer(NULL)
    , m_gameRule(NULL)
{

}

void GameLogic::addEventHandler(const EventHandler *handler)
{
    QList<EventType> events = handler->events();
    foreach(EventType event, events)
        m_handlers[event] << handler;
}

bool GameLogic::trigger(EventType event, Player *target)
{
    QVariant data;
    return trigger(event, target, data);
}

static bool compareByPriority(const EventHandler *a, const EventHandler *b)
{
    return a->priority() > b->priority();
}

bool GameLogic::trigger(EventType event, Player *target, QVariant &data)
{
    QList<const EventHandler *> &handlers = m_handlers[event];

    //@todo: Resolve C++98 Incompatibility?
    qStableSort(handlers.begin(), handlers.end(), [event](const EventHandler *a, const EventHandler *b){
        return a->priority(event) > b->priority(event);
    });

    bool willTriggerEmpty = true;
    int willTriggerPriority = 0;
    QMap<Player *, QList<Event>> triggerableEvents;
    QList<const EventHandler *> triggered;

    int triggerableTested = 0;
    while (triggerableTested < handlers.length()) {
        triggerableEvents.clear();
        willTriggerEmpty = true;

        //Construct triggerableEvents
        foreach (const EventHandler *handler, handlers) {
            if (!triggered.contains(handler)) {
                if (handler == m_gameRule) {
                    if (willTriggerEmpty || handler->priority(event) == willTriggerPriority) {
                        willTriggerEmpty = false;
                        triggerableEvents[NULL] << handler;
                    } else if (handler->priority(event) != willTriggerPriority) {
                        break;
                    }
                } else {
                    if (willTriggerEmpty || handler->priority(event) == willTriggerPriority) {
                        QMap<Player *, Event> Events = handler->triggerable(this, event, target, data);
                        QList<Player *> players = this->players();
                        foreach (Player *p, players) {
                            if (!Events.contains(p))
                                continue;

                            QList<Event> ds = Events.values(p);
                            foreach(const Event &d, ds) {
                                willTriggerEmpty = false;
                                willTriggerPriority = d.handler->priority(event);
                                triggerableEvents[p] << d;
                            }
                        }
                    } else if (handler->priority(event) != willTriggerPriority) {
                        break;
                    }
                }
                triggered.prepend(handler);
            }
            triggerableTested++;
        }

        if (!willTriggerEmpty) {
            QList<Player *> allPlayers = this->allPlayers(true);
            foreach (Player *invoker, allPlayers) {
                if (!triggerableEvents.contains(invoker))
                    continue;

                forever {
                    QList<Event> &events = triggerableEvents[invoker];

                    bool hasCompulsory = false;
                    foreach (const Event &d, events) {
                        if (d.handler->frequency() == EventHandler::Compulsory || d.handler->frequency() == EventHandler::Wake) {
                            hasCompulsory = true;
                            break;
                        }
                    }

                    //Ask the invoker to determine the trigger order
                    Event choice;
                    if (events.length() > 1) {
                        if (!invoker->hasShownBothGenerals())
                            m_globalRequestEnabled = true;
                        choice = askForTriggerOrder(invoker, "GameRule:TriggerOrder", events, !hasCompulsory);
                        m_globalRequestEnabled = false;
                    } else {
                        choice = events.first();
                    }

                    //If the user selects "cancel"
                    if (!choice.isValid())
                        break;

                    //Ask the invoker for cost
                    if (!invoker->hasShownSkill(choice.handler))
                        m_globalRequestEnabled = true;
                    bool takeEffect = choice.handler->cost(this, event, choice.to.at(0), data, invoker);
                    if (takeEffect && !invoker->hasShownSkill(choice.handler)) {
                        //@todo: show head general
                    }
                    m_globalRequestEnabled = false;

                    //Take effect
                    if (takeEffect) {
                        bool broken = choice.handler->effect(this, event, choice.to.at(0), data, invoker);
                        if (broken)
                            break;
                    }

                    //Remove targets that are in front of the triggered target
                    for (int i = 0; i < events.length(); i++) {
                        Event &d = events[i];
                        if (d.handler != choice.handler)
                            continue;

                        foreach (Player *to, choice.to) {
                            int index = d.to.indexOf(to);
                            if (index == d.to.length() - 1) {
                                events.removeAt(i);
                                i--;
                            } else {
                                d.to = d.to.mid(index + 1);
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

QList<Player *> GameLogic::players() const
{
    QList<Player *> players;
    foreach (CAbstractPlayer *p, abstractPlayers())
        players << qobject_cast<Player *>(p);
    return players;
}

Player *GameLogic::findPlayer(uint id) const
{
    return qobject_cast<Player *>(findAbstractPlayer(id));
}

QList<Player *> GameLogic::allPlayers(bool include_dead) const
{
    QList<Player *> players = this->players();
    Player *current = currentPlayer();
    if (current == NULL)
        return players;

    int currentIndex = players.indexOf(current);
    if (currentIndex == -1)
        return players;

    QList<Player *> allPlayers;
    for (int i = currentIndex; i < players.length(); i++) {
        if (include_dead || players.at(i)->isAlive())
            allPlayers << players.at(i);
    }
    for (int i = 0; i < currentIndex; i++) {
        if (include_dead || players.at(i)->isAlive())
            allPlayers << players.at(i);
    }

    if (current->phase() == Player::NotActive && allPlayers.contains(current)) {
        allPlayers.removeOne(current);
        allPlayers.append(current);
    }

    return allPlayers;
}

Event GameLogic::askForTriggerOrder(Player *player, const QString &reason, QList<Event> &options, bool cancelable)
{
    //@todo:
    C_UNUSED(player);
    C_UNUSED(reason);
    C_UNUSED(options);
    C_UNUSED(cancelable);
    return Event();
}

CAbstractPlayer *GameLogic::createPlayer(CServerUser *user)
{
    C_UNUSED(user);
    return new Player(this);
}

CAbstractPlayer *GameLogic::createPlayer(CServerRobot *robot)
{
    C_UNUSED(robot);
    return new Player(this);
}

void GameLogic::run()
{
    qsrand((uint) QDateTime::currentMSecsSinceEpoch());

    //@to-do: Turn broken event not into a new
    trigger(GameStart, NULL);

    forever {
        try {
            forever {
                Player *current = currentPlayer();
                trigger(TurnStart, current);
                Player *next = current->nextAlive(1, false);
                while (!m_extraTurns.isEmpty()) {
                    Player *extra = m_extraTurns.takeFirst();
                    setCurrentPlayer(extra);
                    trigger(TurnStart, extra);
                }
                setCurrentPlayer(next);
            }
        } catch (EventType event) {
            if (event == GameFinish) {
                return;
            } else if (event == TurnBroken) {
                Player *current = currentPlayer();
                trigger(TurnBroken, current);
                Player *next = current->nextAlive(1, false);
                if (current->phase() != Player::NotActive) {
                    QVariant data;
                    m_gameRule->effect(this, PhaseEnd, current, data, current);
                    //@todo:
                    current->setPhase(Player::NotActive);
                }
                setCurrentPlayer(next);
            }
        }
    }
}
