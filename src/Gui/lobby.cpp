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
    , m_currentRoomId(0)
    , m_currentRoomName(tr("QSanguosha Lobby"))
    , m_currentRoomLogo("image://mogara/logo")
{
    CClientPlayer *self = m_client->self();
    if (self) {
        m_userAvatar = self->avatar();
        m_userName = self->screenName();
    }
    foreach (const CClientPlayer *player, m_client->players())
        connect(player, &CClientPlayer::speak, this, &Lobby::onPlayerSpeaking);
    connect(m_client, &Client::roomListUpdated, this, &Lobby::onRoomListUpdated);
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

void Lobby::onRoomListUpdated(const QVariant &list)
{
    emit roomListCleared();
    const QVariantList rooms = list.toList();
    foreach (const QVariant &room, rooms)
        emit roomAdded(room);
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

void Lobby::setCurrentRoomId(uint id)
{
    m_currentRoomId = id;
    emit currentRoomIdChanged();
}

void Lobby::setCurrentRoomName(const QString &name)
{
    m_currentRoomName = name;
    emit currentRoomNameChanged();
}

void Lobby::setCurrentRoomLogo(const QUrl &url)
{
    m_currentRoomLogo = url;
    emit currentRoomLogoChanged();
}

void Lobby::setUserAvatar(const QUrl &avatar)
{
    m_userAvatar = avatar;
    emit userAvatarChanged();
}

void Lobby::setUserName(const QString &name)
{
    m_userName = name;
    emit userNameChanged();
}

void Lobby::Init()
{
    qmlRegisterType<Lobby>("Sanguosha", 1, 0, "Lobby");
}
C_INITIALIZE_CLASS(Lobby)
