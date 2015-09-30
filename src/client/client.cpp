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

static Client *ClientInstance = nullptr;

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
        ClientInstance = nullptr;
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

void Client::useCard(const Card *card, const QList<const Player *> &targets)
{
    QVariantMap data;
    data["cardId"] = card->id();
    QVariantList to;
    foreach (const Player *target, targets)
        to << target->id();
    data["to"] = to;
    replyToServer(S_COMMAND_USE_CARD, data);
}

void Client::respondCard(const QList<const Card *> &cards)
{
    QVariantMap data;

    QVariantList cardData;
    foreach (const Card *card, cards)
        cardData << card->id();
    data["cards"] = cardData;

    replyToServer(S_COMMAND_ASK_FOR_CARD, data);
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

CardArea *Client::findArea(const CardsMoveStruct::Area &area)
{
    if (area.owner) {
        switch (area.type) {
        case CardArea::Hand:
            return area.owner->handcards();
        case CardArea::Equip:
            return area.owner->equips();
        case CardArea::DelayedTrick:
            return area.owner->delayedTricks();
        case CardArea::Judge:
            return area.owner->judgeCards();
        default:
            return nullptr;
        }
    }
    return nullptr;
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

        CClientUser *agent = nullptr;
        if (info.contains("userId")) {
            uint userId = info["userId"].toUInt();
            agent = client->findUser(userId);
        } else if (info.contains("robotId")) {
            uint robotId = info["robotId"].toUInt();
            agent = client->findRobot(robotId);
        }

        if (agent) {
            ClientPlayer *player = new ClientPlayer(agent, client);
            player->setId(info["playerId"].toUInt());
            client->m_players[player->id()] = player;
            client->m_user2player[agent] = player;
            player->setScreenName(agent->screenName());

            players << player;
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

void Client::UpdatePlayerPropertyCommand(QObject *receiver, const QVariant &data)
{
    QVariantList dataList = data.toList();
    if (dataList.length() != 3)
        return;

    Client *client = qobject_cast<Client *>(receiver);
    uint playerId = dataList.at(0).toUInt();
    ClientPlayer *player = client->m_players.value(playerId);
    if (player) {
        QString name = dataList.at(1).toString();
        QVariant value = dataList.at(2);
        player->setProperty(name.toLatin1().constData(), value);
    }
}

void Client::ChooseGeneralRequestCommand(QObject *receiver, const QVariant &data)
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

void Client::MoveCardsCommand(QObject *receiver, const QVariant &data)
{
    Client *client = qobject_cast<Client *>(receiver);

    QList<CardsMoveStruct> moves;

    QVariantList movesData = data.toList();
    foreach (const QVariant &moveVar, movesData) {
        const QVariantMap moveData = moveVar.toMap();
        const QVariantMap from = moveData["from"].toMap();
        const QVariantMap to = moveData["to"].toMap();

        CardsMoveStruct move;
        move.from.type = static_cast<CardArea::Type>(from["type"].toInt());
        move.from.direction = static_cast<CardArea::Direction>(from["direction"].toInt());
        move.from.name = from["name"].toString();
        move.from.owner = client->findPlayer(from["ownerId"].toUInt());
        move.to.type = static_cast<CardArea::Type>(to["type"].toInt());
        move.to.direction = static_cast<CardArea::Direction>(from["direction"].toInt());
        move.to.name = to["name"].toString();
        move.to.owner = client->findPlayer(to["ownerId"].toInt());
        move.isOpen = to["isOpen"].toBool();
        move.isLastHandCard = to["isLastHandCard"].toBool();

        const QVariantList cards = moveData["cards"].toList();
        if (cards.isEmpty()) {
            int cardNum = moveData["cards"].toInt();
            move.cards.reserve(cardNum);
            for (int i = 0; i < cardNum; i++)
                move.cards << nullptr;
        } else {
            foreach (const QVariant &cardData, cards) {
                Card *card = client->m_cards.value(cardData.toUInt());
                if (card)
                    move.cards << card;
                else
                    qWarning("Unknown card id received: %u", cardData.toUInt());
            }
        }

        CardArea *source = client->findArea(move.from);
        CardArea *destination = client->findArea(move.to);
        if (source)
            source->remove(move.cards);
        if (destination)
            destination->add(move.cards);

        moves << move;
    }

    emit client->cardsMoved(moves);
}

void Client::UseCardRequestCommand(QObject *receiver, const QVariant &data)
{
    Client *client = qobject_cast<Client *>(receiver);
    emit client->usingCard(data.toString());
}

void Client::UseCardCommand(QObject *receiver, const QVariant &data)
{
    Client *client = qobject_cast<Client *>(receiver);
    const QVariantMap args = data.toMap();
    const ClientPlayer *from = client->findPlayer(args["from"].toUInt());
    QList<const ClientPlayer *> targets;
    const QVariantList tos = args["to"].toList();
    foreach (const QVariant &to, tos) {
        const ClientPlayer *target = client->findPlayer(to.toUInt());
        if (target)
            targets << target;
    }

    //@to-do: set card footnote
    emit client->cardUsed(from, targets);
}

void Client::AddCardHistoryCommand(QObject *receiver, const QVariant &data)
{
    Client *client = qobject_cast<Client *>(receiver);
    ClientPlayer *self = client->m_user2player.value(client->self());

    if (data.isNull()) {
        self->clearCardHistory();
    } else {
        QVariantList dataList = data.toList();
        if (dataList.size() != 2)
            return;

        QString name = dataList.at(0).toString();
        int times = dataList.at(1).toInt();
        self->addCardHistory(name, times);
    }
}

void Client::DamageCommand(QObject *receiver, const QVariant &data)
{
    QVariantList dataList = data.toList();
    if (dataList.length() != 3)
        return;

    Client *client = qobject_cast<Client *>(receiver);
    ClientPlayer *victim = client->findPlayer(dataList.at(0).toUInt());
    DamageStruct::Nature nature = static_cast<DamageStruct::Nature>(dataList.at(1).toInt());
    int damage = dataList.at(2).toInt();
    emit client->damageDone(victim, nature, damage);
}

void Client::RecoverCommand(QObject *receiver, const QVariant &data)
{
    QVariantMap arg = data.toMap();
    if (!arg.contains("from") || !arg.contains("to") || !arg.contains("num"))
        return;

    Client *client = qobject_cast<Client *>(receiver);
    const ClientPlayer *from = client->findPlayer(arg["from"].toUInt());
    const ClientPlayer *to = client->findPlayer(arg["to"].toUInt());
    int num = arg["num"].toInt();
    emit client->recoverDone(from, to, num);
}

void Client::AskForCardRequestCommand(QObject *receiver, const QVariant &data)
{
    const QVariantMap arg = data.toMap();
    if (!arg.contains("pattern") || !arg.contains("prompt"))
        return;

    QString pattern = arg["pattern"].toString();
    QString prompt = arg["prompt"].toString();
    Client *client = qobject_cast<Client *>(receiver);

    if (arg.contains("minNum") && arg.contains("maxNum")) {
        int minNum = arg["minNum"].toInt();
        int maxNum = arg["maxNum"].toInt();
        int optional = arg["optional"].toBool();
        emit client->cardsAsked(pattern, prompt, minNum, maxNum, optional);
    } else {
        emit client->cardAsked(pattern, prompt);
    }
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
    AddCallback(S_COMMAND_UPDATE_PLAYER_PROPERTY, UpdatePlayerPropertyCommand);
    AddCallback(S_COMMAND_MOVE_CARDS, MoveCardsCommand);
    AddCallback(S_COMMAND_ADD_CARD_HISTORY, AddCardHistoryCommand);
    AddCallback(S_COMMAND_DAMAGE, DamageCommand);
    AddCallback(S_COMMAND_RECOVER, RecoverCommand);
    AddCallback(S_COMMAND_USE_CARD, UseCardCommand);

    AddInteraction(S_COMMAND_CHOOSE_GENERAL, ChooseGeneralRequestCommand);
    AddInteraction(S_COMMAND_USE_CARD, UseCardRequestCommand);
    AddInteraction(S_COMMAND_ASK_FOR_CARD, AskForCardRequestCommand);
}
C_INITIALIZE_CLASS(Client)
