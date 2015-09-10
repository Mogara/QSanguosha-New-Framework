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
#include "engine.h"
#include "player.h"
#include "protocol.h"

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

void Client::restart()
{
    foreach (Player *player, m_players)
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

    QList<Player *> players;
    players.reserve(infos.length());
    for (int i = 0; i < infos.length(); i++)
        players << new Player(client);

    for (int i = 1; i < infos.length(); i++)
        players.at(i - 1)->setNext(players.at(i));
    players.last()->setNext(players.first());

    int i = 0;
    foreach (const QVariant &rawInfo, infos) {
        const QVariantMap info = rawInfo.toMap();
        Player *player = players.at(i);
        player->setId(info["playerId"].toUInt());
        player->setSeat(i + 1);

        client->m_players[player->id()] = player;

        if (info.contains("userId")) {
            uint userId = info["userId"].toUInt();
            CClientUser *user = client->findUser(userId);
            client->m_userMap[player->id()] = user;
        } else if (info.contains("robotId")) {
            //@to-do:
        }
    }
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
