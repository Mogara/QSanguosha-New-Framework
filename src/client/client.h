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
    QList<const ClientPlayer *> players() const;
    int playerNum() const;

signals:
    void seatArranged();
    void chooseGeneralRequested(const QStringList &candidates /* @to-do: add banned pair */);
    void cardsMoved(const QList<CardsMoveStruct> &moves);
    void damageDone(const ClientPlayer *victim, DamageStruct::Nature nature, int damage);

private:
    Client(QObject *parent = 0);

    void restart();
    ClientPlayer *findPlayer(uint id) { return m_players.value(id); }
    Card *findCard(uint id) { return m_cards.value(id); }

    C_DECLARE_INITIALIZER(Client)
    static void ArrangeSeatCommand(QObject *receiver, const QVariant &data);
    static void PrepareCardsCommand(QObject *receiver, const QVariant &data);
    static void ChooseGeneralCommand(QObject *receiver, const QVariant &data);
    static void MoveCardsCommand(QObject *receiver, const QVariant &data);
    static void UseCardCommand(QObject *receiver, const QVariant &data);
    static void AddCardHistoryCommand(QObject *receiver, const QVariant &data);
    static void DamageCommand(QObject *receiver, const QVariant &data);

    QMap<uint, ClientPlayer *> m_players;
    QMap<CClientUser *, ClientPlayer *> m_user2player;
    QMap<uint, Card *> m_cards;//Record card state
};

#endif // CLIENT_H
