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
#include "client.h"
#include "clientplayer.h"
#include "engine.h"
#include "protocol.h"

#include <cclientuser.h>

#include <QtQml>

static Client *ClientInstance = NULL;

Client::Client(QObject *parent)
    : CClient(parent)
{
    ClientInstance = this;
    connect(this, &CClient::gameStarted, this, &Client::restart);
}

Client *Client::instance()
{
    static Client *client = new Client(qApp);
    return client;
}

Client::~Client()
{
    if (ClientInstance == this)
        ClientInstance = NULL;
}

const ClientPlayer *Client::findPlayer(CClientUser *user) const
{
    return m_user2player.value(user);
}

QList<const ClientPlayer *> Client::players() const
{
    QList<const ClientPlayer *> players;
    foreach (const ClientPlayer *player, m_players)
        players << player;
    return players;
}

int Client::playerNum() const
{
    return m_players.size();
}

void Client::restart()
{
    foreach (ClientPlayer *player, m_players)
        player->deleteLater();
    m_players.clear();

    foreach (Card *card, m_cards)
        delete card;
    m_cards.clear();
}

void Client::ArrangeSeatCommand(QObject *receiver, const QVariant &data)
{
    QVariantList infos = data.toList();
    if (infos.isEmpty())
        return;

    Client *client = qobject_cast<Client *>(receiver);

    QList<ClientPlayer *> players;
    players.reserve(infos.length());

    foreach (const QVariant &rawInfo, infos) {
        const QVariantMap info = rawInfo.toMap();
        if (info.contains("userId")) {
            uint userId = info["userId"].toUInt();
            CClientUser *user = client->findUser(userId);

            ClientPlayer *player = new ClientPlayer(user, client);
            player->setId(info["playerId"].toUInt());
            client->m_players[player->id()] = player;
            client->m_user2player[user] = player;
            player->setScreenName(user->screenName());
        } else if (info.contains("robotId")) {
            //@to-do:
        }
    }

    if (!players.isEmpty()) {
        for (int i = 1; i < infos.length(); i++) {
            Player *prev = players.at(i - 1);
            prev->setSeat(i);
            prev->setNext(players.at(i));
        }

        Player *last = players.last();
        last->setSeat(players.length());
        last->setNext(players.first());
    }


    emit client->seatArranged();
}

void Client::PrepareCardsCommand(QObject *receiver, const QVariant &data)
{
    Client *client = qobject_cast<Client *>(receiver);
    Engine *engine = Engine::instance();
    QVariantList cardData = data.toList();
    foreach (const QVariant &cardId, cardData) {
        const Card *card = engine->getCard(cardId.toUInt());
        if (card)
            client->m_cards[card->id()] = card->clone();
    }
}

void Client::ChooseGeneralCommand(QObject *receiver, const QVariant &data)
{
    QVariantList dataList = data.toList();
    if (dataList.length() < 2)
        return;

    QVariantList candidateData = dataList.at(0).toList();

    QStringList generals;
    foreach (const QVariant &general, candidateData)
        generals << general.toString();

    //@to-do: parse banned pairs

    Client *client = qobject_cast<Client *>(receiver);
    emit client->chooseGeneralRequested(generals);
}

static QObject *ClientInstanceCallback(QQmlEngine *, QJSEngine *)
{
    return Client::instance();
}

void Client::Init()
{
    qmlRegisterSingletonType<Client>("Sanguosha", 1, 0, "Client", ClientInstanceCallback);

    AddCallback(S_COMMAND_ARRANGE_SEAT, ArrangeSeatCommand);
    AddCallback(S_COMMAND_PREPARE_CARDS, PrepareCardsCommand);

    AddInteraction(S_COMMAND_CHOOSE_GENERAL, ChooseGeneralCommand);
}
C_INITIALIZE_CLASS(Client)
