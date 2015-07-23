/********************************************************************
    Copyright (c) 2013-2015 - Mogara

    This file is part of Cardirector.

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

#include "lobby.h"
#include "Client/client.h"

#include <cclientplayer.h>

Lobby::Lobby(QQuickItem *parent)
    : QQuickItem(parent)
    , m_client(Client::instance())
{
    connect(m_client, &Client::roomListUpdated, this, &Lobby::onRoomListUpdated);
    connect(m_client, &Client::roomEntered, this, &Lobby::onRoomEntered);
    connect(m_client, &Client::playerAdded, this, &Lobby::onPlayerAdded);
    connect(m_client, &Client::playerRemoved, this, &Lobby::onPlayerRemoved);
}

void Lobby::createRoom()
{
    m_client->createRoom();
}

void Lobby::speakToServer(const QString &text)
{
    m_client->speakToServer(text);
}

void Lobby::updateRoomList()
{
    m_client->fetchRoomList();
}

void Lobby::onCreateButtonClicked()
{
    if (property("roomId").toUInt() == 0)
        m_client->createRoom();
    else
        m_client->exitRoom();
}

void Lobby::onRoomListItemClicked(uint id)
{
    m_client->enterRoom(id);
}

void Lobby::onRoomListUpdated(const QVariant &list)
{
    emit roomListCleared();
    const QVariantList rooms = list.toList();
    foreach (const QVariant &room, rooms)
        emit roomAdded(room);
}

void Lobby::onRoomEntered(const QVariant &config)
{
    QVariantMap info = config.toMap();
    if (!info.isEmpty()) {
        setProperty("roomId", info["id"]);
        setProperty("roomName", info["name"]);
        setProperty("chatLog", "");
    }

    CClientPlayer *self = m_client->self();
    if (self) {
        setProperty("userAvatar", self->avatar());
        setProperty("userName", self->screenName());
    }

    foreach (const CClientPlayer *player, m_client->players())
        connect(player, &CClientPlayer::speak, this, &Lobby::onPlayerSpeaking, Qt::UniqueConnection);
}

void Lobby::onPlayerAdded(const CClientPlayer *player)
{
    connect(player, &CClientPlayer::speak, this, &Lobby::onPlayerSpeaking);
    emit messageLogged(tr("Player %1(%2) logged in.").arg(player->screenName()).arg(player->id()));
}

void Lobby::onPlayerRemoved(const CClientPlayer *player)
{
    disconnect(player, &CClientPlayer::speak, this, &Lobby::onPlayerSpeaking);
    emit messageLogged(tr("Player %1(%2) logged out.").arg(player->screenName()).arg(player->id()));
}

void Lobby::onPlayerSpeaking(const QString &message)
{
    CClientPlayer *player = qobject_cast<CClientPlayer *>(sender());
    if (player == NULL)
        return;
    emit messageLogged(tr("%1(%2): %3").arg(player->screenName()).arg(player->id()).arg(message));
}

void Lobby::Init()
{
    qmlRegisterType<Lobby>("Sanguosha", 1, 0, "Lobby");
}
C_INITIALIZE_CLASS(Lobby)
