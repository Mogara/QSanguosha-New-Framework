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

#ifndef CLIENT_H
#define CLIENT_H

#include <cclient.h>

#include <QMap>

#include "structs.h"

class Card;
class ClientPlayer;

class Client : public CClient
{
    Q_OBJECT

public:
    static Client *instance();
    ~Client();

    const ClientPlayer *findPlayer(CClientUser *user) const;
    const ClientPlayer *selfPlayer() const { return m_user2player.value(self()); }
    QList<const ClientPlayer *> players() const;
    int playerNum() const;

    const Card *findCard(uint id) { return m_cards.value(id); }
    void useCard(const Card *card, const QList<const Player *> &targets);
    void respondCard(const QList<const Card *> &cards);

    const CardArea *wugu() const { return m_wugu; }

signals:
    void promptReceived(const QString &prompt);
    void seatArranged();
    void chooseGeneralRequested(const QStringList &candidates /* @to-do: add banned pair */);
    void cardsMoved(const QList<CardsMoveStruct> &moves);
    void damageDone(const ClientPlayer *victim, DamageStruct::Nature nature, int damage);
    void recoverDone(const ClientPlayer *from, const ClientPlayer *to, int num);
    void usingCard(const QString &pattern, const QList<const Player *> &assignedTarget);
    void cardUsed(const ClientPlayer *from, const QList<const ClientPlayer *> &to);
    void cardAsked(const QString &pattern);
    void cardsAsked(const QString &pattern, int minNum, int maxNum, bool optional);
    void amazingGraceStarted();
    void amazingGraceFinished();
    void amazingGraceRequested();
    void choosePlayerCardRequested(const QList<Card *> &handcards, const QList<Card *> &equips, const QList<Card *> &delayedTricks);

private:
    Client(QObject *parent = 0);

    void restart();
    ClientPlayer *findPlayer(uint id) { return m_players.value(id); }
    CardArea *findArea(const CardsMoveStruct::Area &area);
    QList<Card *> findCards(const QVariant &data);
    static inline QString tr(const QString &text) { return tr(text.toLatin1().constData()); }

    C_DECLARE_INITIALIZER(Client)

    static void ShowPromptCommand(QObject *receiver, const QVariant &data);
    static void ArrangeSeatCommand(QObject *receiver, const QVariant &data);
    static void PrepareCardsCommand(QObject *receiver, const QVariant &data);
    static void UpdatePlayerPropertyCommand(QObject *receiver, const QVariant &data);
    static void ChooseGeneralRequestCommand(QObject *receiver, const QVariant &data);
    static void MoveCardsCommand(QObject *receiver, const QVariant &data);
    static void UseCardRequestCommand(QObject *receiver, const QVariant &data);
    static void UseCardCommand(QObject *receiver, const QVariant &data);
    static void AddCardHistoryCommand(QObject *receiver, const QVariant &data);
    static void DamageCommand(QObject *receiver, const QVariant &data);
    static void RecoverCommand(QObject *receiver, const QVariant &data);
    static void AskForCardRequestCommand(QObject *receiver, const QVariant &data);
    static void ShowAmazingGraceCommand(QObject *receiver, const QVariant &);
    static void ClearAmazingGraceCommand(QObject *receiver, const QVariant &);
    static void TakeAmazingGraceRequestCommand(QObject *receiver, const QVariant &data);
    static void ChoosePlayerCardRequestCommand(QObject *receiver, const QVariant &data);

    QMap<uint, ClientPlayer *> m_players;
    QMap<CClientUser *, ClientPlayer *> m_user2player;
    QMap<uint, Card *> m_cards;//Record card state

    CardArea *m_wugu;
};

#endif // CLIENT_H
