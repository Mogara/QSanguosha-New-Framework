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

//#include "event.h"
#include "datavalue.h"
//#include "eventhandler.h"

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
    explicit GameLogic(CRoom *parent = 0);
    ~GameLogic();

    const RoomSettings *settings() const;

    void setGameRule(const GameRule *rule);

//    QList<const Package *> packages() const { return m_packages; }
//    void setPackages(const QList<const Package *> &packages) { m_packages = packages; }
//    void setPackages(const QStringList &packages);

    void addEventHandler(const EventHandler *handler);
    void removeEventHandler(const EventHandler *handler);
    bool trigger(/*EventHandler::EventType*/ int event, ServerPlayer *target, QObject *data = nullptr);

//    void setCurrentPlayer(ServerPlayer *player) { m_currentPlayer = player; }
//    ServerPlayer *currentPlayer() const { return m_currentPlayer; }

    Q_INVOKABLE QList<ServerPlayer *> players() const;
    Q_INVOKABLE ServerPlayer *findPlayer(uint id) const;
    Q_INVOKABLE ServerPlayer *findPlayer(CServerAgent *agent) const;

    Q_INVOKABLE QList<ServerPlayer *> allPlayers(bool includeDead = false) const;
    Q_INVOKABLE QList<ServerPlayer *> otherPlayers(ServerPlayer *except, bool includeDead = false) const;
    Q_INVOKABLE void sortByActionOrder(QList<ServerPlayer *> &players) const;

//    Q_INVOKABLE void addExtraTurn(ServerPlayer *player) { m_extraTurns << player; }
//    QList<ServerPlayer *> extraTurns() const { return m_extraTurns; }

//    bool skipGameRule() const { return m_skipGameRule; }

    Card *getDrawPileCard();
    QList<Card *> getDrawPileCards(int n);
    void reshuffleDrawPile();
//    int reshufflingCount() const { return m_reshufflingCount; }

    Q_INVOKABLE void moveCards(DataValue::CardsMoveValue &move);
    //void moveCards(QList<CardsMoveStruct> &moves);

    Q_INVOKABLE bool useCard(DataValue::CardUseValue &use);
    Q_INVOKABLE bool takeCardEffect(DataValue::CardEffectValue &effect);
    Q_INVOKABLE bool invokeProactiveSkill(DataValue::SkillInvokeValue &invoke);

    //It returns false if the corresponding CardResponded event is broken.
    Q_INVOKABLE bool respondCard(DataValue::CardResponseValue &response);

    Q_INVOKABLE void judge(DataValue::JudgeValue &judge);
    void retrialCost(DataValue::JudgeValue &judge, Card *card, bool isReplace = false);
    void retrialEffect(DataValue::JudgeValue &judge, Card *card);

//    Card *findCard(uint id) const { return m_cards.value(id); }
    QList<Card *> findCards(const QVariant &data);

    Q_INVOKABLE void damage(DataValue::DamageValue &damage);
    Q_INVOKABLE void loseHp(ServerPlayer *victim, int lose);
    Q_INVOKABLE void recover(DataValue::RecoverValue &recover);

    Q_INVOKABLE void killPlayer(ServerPlayer *victim, DataValue::DamageValue *damage = nullptr);
    Q_INVOKABLE void gameOver(const QList<ServerPlayer *> &winners);

    QMap<uint, QList<const General *> > broadcastRequestForGenerals(const QList<ServerPlayer *> &players, int num, int limit);

    Q_INVOKABLE ServerPlayer *getFront(ServerPlayer *a, ServerPlayer *b) const;

protected:
    CAbstractPlayer *createPlayer(CServerAgent *agent) override;

    void loadMode(const GameMode *mode);

    void prepareToStart();
    void filterCardsMove(DataValue::CardsMoveValue &moves);

    void getEventHandlersAndSort(/*EventHandler::EventType*/ int event, /*EventPtrList*/ int &detailsList, const /*EventPtrList*/ int &triggered, const QObject *data, ServerPlayer *player = nullptr);

    void run();
};

#endif // CGAMELOGIC_H
