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

#include <CAbstractGameLogic>

class Card;
class CardArea;
class GameRule;
class GameMode;
class ServerPlayer;
class Package;
class RoomSettings;

class GameLogic : public CAbstractGameLogic
{
    Q_OBJECT

public:
    GameLogic(CRoom *parent = 0);
    ~GameLogic();

    const RoomSettings *settings() const;

    void setGameRule(const GameRule *rule);

    QList<const Package *> packages() const { return m_packages; }
    void setPackages(const QList<const Package *> &packages) { m_packages = packages; }

    void addEventHandler(const EventHandler *handler);
    void removeEventHandler(const EventHandler *handler);
    bool trigger(EventType event, ServerPlayer *target);
    bool trigger(EventType event, ServerPlayer *target, QVariant &data);

    void setCurrentPlayer(ServerPlayer *player) { m_currentPlayer = player; }
    ServerPlayer *currentPlayer() const { return m_currentPlayer; }

    QList<ServerPlayer *> players() const;
    ServerPlayer *findPlayer(uint id) const;
    ServerPlayer *findPlayer(CServerAgent *agent) const;

    QList<ServerPlayer *> allPlayers(bool includeDead = false) const;
    QList<ServerPlayer *> otherPlayers(ServerPlayer *except, bool includeDead = false) const;
    void sortByActionOrder(QList<ServerPlayer *> &players) const;

    void addExtraTurn(ServerPlayer *player) { m_extraTurns << player; }
    QList<ServerPlayer *> extraTurns() const { return m_extraTurns; }

    bool skipGameRule() const { return m_skipGameRule; }

    Card *getDrawPileCard();
    QList<Card *> getDrawPileCards(int n);
    void reshuffleDrawPile();
    int reshufflingCount() const { return m_reshufflingCount; }

    CardArea *drawPile() const { return m_drawPile; }
    CardArea *discardPile() const { return m_discardPile; }
    CardArea *table() const { return m_table; }
    CardArea *wugu() const { return m_wugu; }

    void moveCards(CardsMoveStruct &move);
    void moveCards(QList<CardsMoveStruct> &moves);

    bool useCard(CardUseStruct &use);
    bool takeCardEffect(CardEffectStruct &effect);

    bool invokeProactiveSkill(SkillInvokeStruct &invoke);

    //It returns false if the corresponding CardResponded event is broken.
    bool respondCard(CardResponseStruct &response);

    void judge(JudgeStruct &judge);
    void retrialCost(JudgeStruct &judge, Card *card, bool isReplace = false);
    void retrialEffect(JudgeStruct &judge, Card *card);

    Card *findCard(uint id) const { return m_cards.value(id); }
    QList<Card *> findCards(const QVariant &data);

    void damage(DamageStruct &damage);
    void loseHp(ServerPlayer *victim, int lose);
    void recover(RecoverStruct &recover);

    void killPlayer(ServerPlayer *victim, DamageStruct *damage = nullptr);
    void gameOver(const QList<ServerPlayer *> &winners);

    QMap<uint, QList<const General *> > broadcastRequestForGenerals(const QList<ServerPlayer *> &players, int num, int limit);

    ServerPlayer *getFront(ServerPlayer *a, ServerPlayer *b) const;

protected:
    CAbstractPlayer *createPlayer(CServerUser *user);
    CAbstractPlayer *createPlayer(CServerRobot *robot);

    void loadMode(const GameMode *mode);

    void prepareToStart();
    CardArea *findArea(const CardsMoveStruct::Area &area);
    void filterCardsMove(QList<CardsMoveStruct> &moves);

    void getEventHandlersAndSort(EventType event, EventPtrList &detailsList, const EventPtrList &triggered, const QVariant &data, ServerPlayer *player = nullptr);

    void run();

private:
    QList<const EventHandler *> m_handlers[EventTypeCount];
    QList<ServerPlayer *> m_players;
    ServerPlayer *m_currentPlayer;
    QList<ServerPlayer *> m_extraTurns;
    const GameRule *m_gameRule;
    QList<const Package *> m_packages;
    QMap<uint, Card *> m_cards;
    bool m_skipGameRule;
    int m_round;
    int m_reshufflingCount;

    CardArea *m_drawPile;
    CardArea *m_discardPile;
    CardArea *m_table;
    CardArea *m_wugu;

    QMap<Card *, CardArea *> m_cardPosition;
};

#endif // CGAMELOGIC_H
