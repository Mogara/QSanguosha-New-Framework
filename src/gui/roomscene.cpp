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

#include "cglobal.h"
#include "client.h"
#include "player.h"
#include "protocol.h"
#include "roomscene.h"
#include "util.h"

RoomScene::RoomScene(QQuickItem *parent)
    : QQuickItem(parent)
    , m_client(Client::instance())
{
    connect(m_client, &Client::chooseGeneralRequested, this, &RoomScene::onChooseGeneralRequested);
    connect(m_client, &Client::seatArranged, this, &RoomScene::onSeatArranged);
    connect(this, &RoomScene::chooseGeneralFinished, this, &RoomScene::onChooseGeneralFinished);
}

void RoomScene::onSeatArranged()
{
    QList<const Player *> players = m_client->players();
    players.removeOne(m_client->findPlayer(m_client->self()));
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
