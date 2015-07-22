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

Lobby::Lobby(QQuickItem *parent)
    : QQuickItem(parent)
    , m_currentRoomId(0)
    , m_currentRoomName(tr("QSanguosha Lobby"))
    , m_currentRoomLogo("image://mogara/logo")
{
}

void Lobby::createRoom()
{

}

void Lobby::speakToServer(const QString &text)
{

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

void Lobby::Init()
{
    qmlRegisterType<Lobby>("Sanguosha", 1, 0, "Lobby");
}
C_INITIALIZE_CLASS(Lobby)
