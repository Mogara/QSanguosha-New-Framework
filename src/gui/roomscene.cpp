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
#include "cglobal.h"
#include "client.h"
#include "clientplayer.h"
#include "protocol.h"
#include "roomscene.h"
#include "util.h"

static QString AreaTypeToString(CardArea::Type type)
{
    switch (type) {
    case CardArea::Judge:
    case CardArea::Table:
    case CardArea::DiscardPile:
        return "table";
    case CardArea::DrawPile:
        return "drawPile";
    case CardArea::Hand:
        return "hand";
    case CardArea::Equip:
        return "equip";
    case CardArea::DelayedTrick:
        return "delayedTrick";
    default:
        return "unknown";
    }
}

static QVariant AreaToVariant(const CardsMoveStruct::Area area)
{
    QVariantMap data;
    data["seat"] = area.owner ? area.owner->seat() : 0;
    data["type"] = AreaTypeToString(area.type);
    data["pile"] = area.pile;
    return data;
}

RoomScene::RoomScene(QQuickItem *parent)
    : QQuickItem(parent)
    , m_client(Client::instance())
{
    connect(m_client, &Client::chooseGeneralRequested, this, &RoomScene::onChooseGeneralRequested);
    connect(m_client, &Client::seatArranged, this, &RoomScene::onSeatArranged);
    connect(m_client, &Client::cardsMoved, this, &RoomScene::animateCardsMoving);
    connect(this, &RoomScene::chooseGeneralFinished, this, &RoomScene::onChooseGeneralFinished);
}

void RoomScene::animateCardsMoving(const QList<CardsMoveStruct> &moves)
{
    QVariantList paths;
    foreach (const CardsMoveStruct &move, moves) {
        QVariantMap path;
        path["from"] = AreaToVariant(move.from);
        path["to"] = AreaToVariant(move.to);

        QVariantList cards;
        foreach (const Card *card, move.cards) {
            QVariantMap cardData;
            if (card) {
                cardData["cid"] = card->id();
                cardData["name"] = card->objectName();
                cardData["number"] = card->number();
                cardData["suit"] = card->suitString();
            } else {
                cardData["cid"] = 0;
                cardData["name"] = "hegback";
                cardData["number"] = 0;
                cardData["suit"] = "";
            }
            cards << cardData;
        }
        path["cards"] = cards;
        paths << path;
    }

    emit cardsMoved(paths);
}

void RoomScene::onSeatArranged()
{
    QList<const ClientPlayer *> players = m_client->players();
    const ClientPlayer *self = m_client->findPlayer(m_client->self());
    players.removeOne(self);
    setProperty("dashboardSeat", self->seat());
    setProperty("photoModel", qConvertToModel(players));
    setProperty("playerNum", players.length() + 1);
}

void RoomScene::onChooseGeneralRequested(const QStringList &candidates)
{
    QVariantList generals;
    foreach (const QString &candidate, candidates) {
        QVariantMap general;
        general["name"] = candidate;
        //@to-do: resolve this
        general["kingdom"] = "shu";
        generals << general;
    }
    emit chooseGeneralStarted(generals);
}

void RoomScene::onChooseGeneralFinished(const QString &head, const QString &deputy)
{
    QVariantList data;
    data << head;
    data << deputy;
    m_client->replyToServer(S_COMMAND_CHOOSE_GENERAL, data);
}

C_REGISTER_QMLTYPE("Sanguosha", 1, 0, RoomScene)
