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
#include "cardarea.h"
#include "eventhandler.h"
#include "gamelogic.h"
#include "gamerule.h"
#include "general.h"
#include "package.h"
#include "protocol.h"
#include "serverplayer.h"
#include "util.h"

#include <croom.h>
#include <cserver.h>
#include <cserveruser.h>
#include <cserverrobot.h>

#include <QDateTime>
#include <QThread>

GameLogic::GameLogic(CRoom *parent)
    : CAbstractGameLogic(parent)
    , m_currentPlayer(nullptr)
    , m_gameRule(nullptr)
    , m_skipGameRule(false)
    , m_round(0)
{
    m_drawPile = new CardArea(CardArea::DrawPile);
    m_discardPile = new CardArea(CardArea::DiscardPile);
    m_table = new CardArea(CardArea::Table);
}

GameLogic::~GameLogic()
{
    delete m_table;
    delete m_discardPile;
    delete m_drawPile;

    foreach (Card *card, m_cards)
        delete card;
}

void GameLogic::setGameRule(const GameRule *rule) {
    if (m_gameRule) {
        foreach (EventType e, m_gameRule->events()) {
            if (m_handlers[e].contains(m_gameRule))
                m_handlers[e].removeOne(m_gameRule);
        }
    }

    m_gameRule = rule;
    if (rule) {
        foreach (EventType e, rule->events())
            m_handlers[e].append(m_gameRule);
    }
}

void GameLogic::addEventHandler(const EventHandler *handler)
{
    QList<EventType> events = handler->events();
    foreach(EventType event, events)
        m_handlers[event] << handler;
}

bool GameLogic::trigger(EventType event, ServerPlayer *target)
{
    QVariant data;
    return trigger(event, target, data);
}

bool GameLogic::trigger(EventType event, ServerPlayer *target, QVariant &data)
{
    QList<const EventHandler *> &handlers = m_handlers[event];

    //@todo: Resolve C++98 Incompatibility?
    qStableSort(handlers.begin(), handlers.end(), [event](const EventHandler *a, const EventHandler *b){
        return a->priority(event) > b->priority(event);
    });

    bool broken = false;
    int triggerableIndex = 0;
    while (triggerableIndex < handlers.length()) {
        int currentPriority = 0;
        QMap<ServerPlayer *, QList<Event>> triggerableEvents;

        //Construct triggerableEvents
        do {
            const EventHandler *handler = handlers.at(triggerableIndex);
            if (triggerableEvents.isEmpty() || handler->priority(event) == currentPriority) {
                QMap<ServerPlayer *, Event> events = handler->triggerable(this, event, target, data);
                QList<ServerPlayer *> players = this->players();
                foreach (ServerPlayer *p, players) {
                    if (!events.contains(p))
                        continue;

                    QList<Event> ds = events.values(p);
                    triggerableEvents[p] << ds;
                    currentPriority = ds.last().handler->priority(event);
                }
            } else if (handler->priority(event) != currentPriority) {
                break;
            }
            triggerableIndex++;
        } while (triggerableIndex < handlers.length());

        if (!triggerableEvents.isEmpty()) {
            QList<ServerPlayer *> allPlayers = this->allPlayers(true);
            foreach (ServerPlayer *invoker, allPlayers) {
                if (!triggerableEvents.contains(invoker))
                    continue;

                forever {
                    QList<Event> &events = triggerableEvents[invoker];
                    if (events.isEmpty())
                        break;

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
                        choice = invoker->askForTriggerOrder("GameRule:TriggerOrder", events, !hasCompulsory);
                        m_globalRequestEnabled = false;
                    } else {
                        choice = events.first();
                    }

                    //If the user selects "cancel"
                    if (!choice.isValid())
                        break;

                    ServerPlayer *eventTarget = choice.to.isEmpty() ? target : choice.to.first();

                    //Ask the invoker for cost
                    if (!invoker->hasShownSkill(choice.handler))
                        m_globalRequestEnabled = true;
                    bool takeEffect = choice.handler->cost(this, event, eventTarget, data, invoker);
                    if (takeEffect && !invoker->hasShownSkill(choice.handler)) {
                        //@todo: show skill here?
                    }
                    m_globalRequestEnabled = false;

                    //Take effect
                    if (takeEffect) {
                        broken = choice.handler->effect(this, event, eventTarget, data, invoker);
                        if (broken)
                            break;
                    }

                    //Remove targets that are in front of the triggered target
                    for (int i = 0; i < events.length(); i++) {
                        Event &d = events[i];
                        if (d.handler != choice.handler)
                            continue;

                        foreach (ServerPlayer *to, choice.to) {
                            int index = d.to.indexOf(to);
                            if (index == d.to.length() - 1) {
                                events.removeAt(i);
                                i--;
                            } else {
                                d.to = d.to.mid(index + 1);
                            }
                        }

                        if (choice.to.isEmpty()) {
                            events.removeAt(i);
                            i--;
                        }
                    }
                }
            }
        }
    }

    return broken;
}

QList<ServerPlayer *> GameLogic::players() const
{
    QList<ServerPlayer *> players;
    auto abstractPlayers = this->abstractPlayers();
    foreach (CAbstractPlayer *p, abstractPlayers)
        players << qobject_cast<ServerPlayer *>(p);
    return players;
}

ServerPlayer *GameLogic::findPlayer(uint id) const
{
    return qobject_cast<ServerPlayer *>(findAbstractPlayer(id));
}

ServerPlayer *GameLogic::findPlayer(CServerAgent *agent) const
{
    return qobject_cast<ServerPlayer *>(findAbstractPlayer(agent));
}

QList<ServerPlayer *> GameLogic::allPlayers(bool includeDead) const
{
    QList<ServerPlayer *> players = this->players();
    ServerPlayer *current = currentPlayer();
    if (current == nullptr)
        return players;

    int currentIndex = players.indexOf(current);
    if (currentIndex == -1)
        return players;

    QList<ServerPlayer *> allPlayers;
    for (int i = currentIndex; i < players.length(); i++) {
        if (includeDead || players.at(i)->isAlive())
            allPlayers << players.at(i);
    }
    for (int i = 0; i < currentIndex; i++) {
        if (includeDead || players.at(i)->isAlive())
            allPlayers << players.at(i);
    }

    if (current->phase() == Player::Inactive && allPlayers.contains(current)) {
        allPlayers.removeOne(current);
        allPlayers.append(current);
    }

    return allPlayers;
}

QList<ServerPlayer *> GameLogic::otherPlayers(ServerPlayer *except, bool includeDead) const
{
    QList<ServerPlayer *> players = allPlayers(includeDead);
    if (except && (except->isAlive() || includeDead))
        players.removeOne(except);
    return players;
}

void GameLogic::sortByActionOrder(QList<ServerPlayer *> &players) const
{
    QList<ServerPlayer *> allPlayers = this->allPlayers(true);

    QMap<ServerPlayer *, int> actionOrder;
    foreach (ServerPlayer *player, players)
        actionOrder[player] = allPlayers.indexOf(player);

    qSort(allPlayers.begin(), allPlayers.end(), [&actionOrder](ServerPlayer *a, ServerPlayer *b){
        return actionOrder.value(a) < actionOrder.value(b);
    });
}

void GameLogic::moveCards(const CardsMoveStruct &move)
{
    moveCards(QList<CardsMoveStruct>() << move);
}

void GameLogic::moveCards(QList<CardsMoveStruct> moves)
{
    //Fill card source information
    for (int i = 0, maxi = moves.length(); i < maxi; i++) {
        CardsMoveStruct &move = moves[i];
        if (move.from.type != CardArea::Unknown)
            continue;

        QMap<CardArea *, QList<Card *>> cardSource;
        foreach (Card *card, move.cards) {
            CardArea *from = m_cardPosition[card];
            if (from == nullptr)
                continue;
            cardSource[from].append(card);
        }

        QMapIterator<CardArea *, QList<Card *>> iter(cardSource);
        while (iter.hasNext()) {
            iter.next();
            CardArea *from = iter.key();
            CardsMoveStruct submove;
            submove.from.type = from->type();
            submove.from.owner = from->owner();
            submove.from.name = from->name();
            submove.cards = iter.value();
            submove.to = move.to;
            submove.isOpen = move.isOpen;
            moves << submove;
        }

        moves.removeAt(i);
        i--;
        maxi--;
    }

    QList<ServerPlayer *> allPlayers = this->allPlayers();
    QVariant moveData = QVariant::fromValue(&moves);
    foreach (ServerPlayer *player, allPlayers)
        trigger(BeforeCardsMove, player, moveData);

    for (int i = 0 ; i < moves.length(); i++) {
        const CardsMoveStruct &move = moves.at(i);
        CardArea *from = findArea(move.from);
        CardArea *to = findArea(move.to);
        if (from == nullptr || to == nullptr)
            continue;

        foreach (Card *card, move.cards) {
            if (from != m_cardPosition.value(card))
                continue;
            if (from->remove(card)) {
                to->add(card, move.to.direction);
                m_cardPosition[card] = to;
            }
        }
    }

    QList<ServerPlayer *> viewers = players();
    foreach (ServerPlayer *viewer, viewers) {
        QVariantList data;
        foreach (const CardsMoveStruct &move, moves)
            data << move.toVariant(move.isRelevant(viewer));
        CServerAgent *agent = viewer->agent();
        agent->notify(S_COMMAND_MOVE_CARDS, data);
    }

    allPlayers = this->allPlayers();
    foreach (ServerPlayer *player, allPlayers)
        trigger(CardsMove, player, moveData);
}

bool GameLogic::useCard(CardUseStruct &use)
{
    if (use.card == nullptr || use.from == nullptr)
        return false;

    //Initialize isHandcard
    use.isHandcard = true;
    QList<Card *> realCards = use.card->realCards();
    foreach (Card *card, realCards) {
        CardArea *area = m_cardPosition[card];
        if (area == nullptr || area->owner() != use.from || area->type() != CardArea::Hand) {
            use.isHandcard = false;
            break;
        }
    }

    if (use.from->phase() == Player::Play && use.addHistory)
        use.from->addCardHistory(use.card->objectName());

    try {
        use.card->onUse(this, use);

        QVariant data = QVariant::fromValue(&use);
        trigger(CardUsed, use.from, data);

        if (use.from) {
            trigger(TargetChoosing, use.from, data);

            QVariantMap args;
            args["from"] = use.from->id();
            //args["cards"]
            QVariantList tos;
            foreach (ServerPlayer *to, use.to)
                tos << to->id();
            args["to"] = tos;
            room()->broadcastNotification(S_COMMAND_USE_CARD, args);

            if (use.from && (!use.to.isEmpty() || use.target)) {
                foreach (ServerPlayer *to, use.to) {
                    if (!use.to.contains(to))
                        continue;
                    trigger(TargetConfirming, to, data);
                }

                if (use.from && (!use.to.isEmpty() || use.target)) {
                    trigger(TargetChosen, use.from, data);

                    if (use.from && (!use.to.isEmpty() || use.target)) {
                        foreach (ServerPlayer *to, use.to) {
                            if (!use.to.contains(to))
                                continue;
                            trigger(TargetConfirmed, to, data);
                        }

                        use.card->use(this, use);
                    }
                }
            }
        }

        trigger(CardFinished, use.from, data);

    } catch (EventType e) {
        //@to-do: handle TurnBroken and StageChange
        throw e;
    }

    return true;
}

bool GameLogic::takeCardEffect(CardEffectStruct &effect)
{
    QVariant data = QVariant::fromValue(&effect);
    bool canceled = false;
    if (effect.to) {
        if (effect.to->isAlive()) {
            // No skills should be triggered here!
            trigger(CardEffect, effect.to, data);
            // Make sure that effectiveness of Slash isn't judged here!
            canceled = trigger(CardEffected, effect.to, data);
            if (!canceled) {
                trigger(CardEffectConfirmed, effect.to, data);
                if (effect.to->isAlive())
                    effect.card->onEffect(this, effect);
            }
        }
    } else if (effect.target)
        effect.card->onEffect(this, effect);
    trigger(PostCardEffected, effect.to, data);
    return !canceled;
}

QList<Card *> GameLogic::findCards(const QVariant &data)
{
    QList<Card *> cards;
    QVariantList dataList = data.toList();
    foreach (const QVariant &cardId, dataList) {
        Card *card = findCard(cardId.toUInt());
        if (card)
            cards << card;
    }
    return cards;
}

void GameLogic::damage(DamageStruct &damage)
{
    if (damage.to == NULL || damage.to->isDead())
        return;

    QVariant data = QVariant::fromValue(&damage);
    if (!damage.chain && !damage.transfer) {
        trigger(ConfirmDamage, damage.from, data);
    }

    // Predamage
    if (trigger(Predamage, damage.from, data))
        return;

    try {
        do {
            if (trigger(DamageForseen, damage.to, data))
                break;

            if (damage.from && trigger(DamageCaused, damage.from, data))
                break;

            if (damage.to && trigger(DamageInflicted, damage.to, data))
                break;
        } while (false);

        if (damage.to)
            trigger(PreDamageDone, damage.to, data);

        if (damage.to && !trigger(DamageDone, damage.to, data)) {
            QVariantList arg;
            arg << damage.to->id();
            arg << damage.nature;
            arg << damage.damage;
            room()->broadcastNotification(S_COMMAND_DAMAGE, arg);

            int newHp = damage.to->hp() - damage.damage;
            damage.to->setHp(newHp);
            damage.to->broadcastProperty("hp");
        }

        if (damage.from)
            trigger(Damage, damage.from, data);

        if (damage.to)
            trigger(Damaged, damage.to, data);

        if (damage.to)
            trigger(DamageComplete, damage.to, data);

    } catch (EventType e) {
        //@to-do: handle TurnBroken and StageChange
        throw e;
    }
}

void GameLogic::recover(RecoverStruct &recover)
{
    if (recover.to == nullptr || recover.to->lostHp() == 0 || recover.to->isDead())
        return;

    QVariant data = QVariant::fromValue(&recover);
    if (trigger(PreHpRecover, recover.to, data))
        return;
    if (recover.to == nullptr)
        return;

    int newHp = qMin(recover.to->hp() + recover.recover, recover.to->maxHp());
    recover.to->setHp(newHp);
    recover.to->broadcastProperty("hp");

    QVariantMap arg;
    arg["from"] = recover.from ? recover.from->id() : 0;
    arg["to"] = recover.to->id();
    arg["num"] = recover.recover;
    room()->broadcastNotification(S_COMMAND_RECOVER, arg);

    trigger(HpRecover, recover.to, data);
}

void GameLogic::delay(ulong msecs)
{
    QThread::currentThread()->msleep(msecs);
}

CAbstractPlayer *GameLogic::createPlayer(CServerUser *user)
{
    return new ServerPlayer(this, user);
}

CAbstractPlayer *GameLogic::createPlayer(CServerRobot *robot)
{
    return new ServerPlayer(this, robot);
}

void GameLogic::prepareToStart()
{
    CRoom *room = this->room();

    //Arrange seats for all the players
    QList<ServerPlayer *> players = this->players();
    qShuffle(players);
    for (int i = 1; i < players.length(); i++) {
        players[i - 1]->setSeat(i);
        players[i - 1]->setNext(players.at(i));
    }
    ServerPlayer *lastPlayer = players.last();
    lastPlayer->setSeat(players.length());
    lastPlayer->setNext(players.first());
    setCurrentPlayer(players.first());

    QVariantList playerList;
    foreach (ServerPlayer *player, players) {
        CServerAgent *agent = findAgent(player);
        QVariantMap info;
        if (agent->controlledByClient()) {
            info["userId"] = agent->id();
        } else {
            info["robotId"] = agent->id();
        }
        info["playerId"] = player->id();
        playerList << info;
    }
    room->broadcastNotification(S_COMMAND_ARRANGE_SEAT, playerList);

    //Import packages
    QList<const General *> generals;
    foreach (const Package *package, m_packages) {
        generals << package->generals();
        QList<const Card *> cards = package->cards();
        foreach (const Card *card, cards)
            m_cards.insert(card->id(), card->clone());
    }

    //Prepare cards
    QVariantList cardData;
    foreach (const Card *card, m_cards)
        cardData << card->id();
    room->broadcastNotification(S_COMMAND_PREPARE_CARDS, cardData);

    //Choose 7 random generals for each player
    //@to-do: config
    int candidateLimit = 7;
    qShuffle(generals);

    QMap<ServerPlayer *, QList<const General *>> playerCandidates;

    foreach (ServerPlayer *player, players) {
        QList<const General *> candidates = generals.mid((player->seat() - 1) * candidateLimit, candidateLimit);
        playerCandidates[player] = candidates;

        QVariantList candidateData;
        foreach (const General *general, candidates)
            candidateData << general->name();

        QVariantList bannedPairData;
        //@todo: load banned pairs

        QVariantList data;
        data << QVariant(candidateData);
        data << QVariant(bannedPairData);

        CServerAgent *agent = findAgent(player);
        agent->prepareRequest(S_COMMAND_CHOOSE_GENERAL, data);
    }

    //@to-do: timeout should be loaded from config
    room->broadcastRequest(room->agents(), 15000);

    foreach (ServerPlayer *player, players) {
        const QList<const General *> &candidates = playerCandidates[player];
        QList<const General *> generals;

        CServerAgent *agent = findAgent(player);
        if (agent) {
            QVariantList reply = agent->waitForReply(0).toList();
            foreach (const QVariant &choice, reply) {
                QString name = choice.toString();
                foreach (const General *general, candidates) {
                    if (general->name() == name)
                        generals << general;
                }
            }
        }

        //@to-do: handle banned pairs
        if (generals.length() < 2)
            generals = candidates.mid(0, 2);

        player->setHeadGeneral(generals.at(0));
        player->setDeputyGeneral(generals.at(1));
    }

    foreach (Card *card, m_cards) {
        m_drawPile->add(card);
        m_cardPosition[card] = m_drawPile;
    }
    qShuffle(m_drawPile->cards());
}

CardArea *GameLogic::findArea(const CardsMoveStruct::Area &area)
{
    if (area.owner) {
        switch (area.type) {
        case CardArea::Hand: {
            ServerPlayer *owner = findPlayer(area.owner->id());
            return owner->handcards();
        }
        case CardArea::Equip:
            return area.owner->equips();
        case CardArea::DelayedTrick:
            return area.owner->delayedTricks();
        case CardArea::Judge:
            return area.owner->judgeCards();
        default: qWarning("Owner Area Not Found");
        }
    } else {
        switch (area.type) {
        case CardArea::DrawPile:
            return m_drawPile;
        case CardArea::DiscardPile:
            return m_discardPile;
        case CardArea::Table:
            return m_table;
        default: qWarning("Global Area Not Found");
        }
    }
    return nullptr;
}

void GameLogic::run()
{
    qsrand((uint) QDateTime::currentMSecsSinceEpoch());

    prepareToStart();

    //@to-do: Turn broken event
    QList<ServerPlayer *> allPlayers = this->allPlayers();
    foreach (ServerPlayer *player, allPlayers)
        trigger(GameStart, player);

    forever {
        try {
            forever {
                ServerPlayer *current = currentPlayer();
                if (current->seat() == 1)
                    m_round++;

                trigger(TurnStart, current);
                ServerPlayer *next = current->nextAlive(1, false);
                while (!m_extraTurns.isEmpty()) {
                    ServerPlayer *extra = m_extraTurns.takeFirst();
                    setCurrentPlayer(extra);
                    trigger(TurnStart, extra);
                }
                setCurrentPlayer(next);
            }
        } catch (EventType event) {
            if (event == GameFinish) {
                return;
            } else if (event == TurnBroken) {
                ServerPlayer *current = currentPlayer();
                trigger(TurnBroken, current);
                ServerPlayer *next = current->nextAlive(1, false);
                if (current->phase() != Player::Inactive) {
                    QVariant data;
                    m_gameRule->effect(this, PhaseEnd, current, data, current);
                    //@todo:
                    current->setPhase(Player::Inactive);
                    current->broadcastProperty("phase");
                }
                setCurrentPlayer(next);
            }
        }
    }
}
