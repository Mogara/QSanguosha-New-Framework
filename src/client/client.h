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

class Card;
class CClientPlayer;
class Player;

class Client : public CClient
{
    Q_OBJECT

public:
    static Client *instance();
    ~Client();

signals:
    void chooseGeneral(const QStringList &candidates /* @to-do: add banned pair */);

private:
    Client(QObject *parent = 0);

    void restart();

    C_DECLARE_INITIALIZER(Client)
    static void ArrangeSeatCommand(QObject *receiver, const QVariant &data);
    static void PrepareCardsCommand(QObject *receiver, const QVariant &data);
    static void ChooseGeneralCommand(QObject *receiver, const QVariant &data);

    QMap<uint, Player *> m_players;
    QMap<uint, CClientUser *> m_userMap;//Player Id to CClient User
    QMap<uint, Card *> m_cards;//Record card state
};

#endif // CLIENT_H
